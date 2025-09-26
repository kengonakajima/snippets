const startButton = document.getElementById('start-button');
const stopButton = document.getElementById('stop-button');
const statusLabel = document.getElementById('status');
const transcriptEl = document.getElementById('transcript');
const clientMeterFill = document.getElementById('client-meter');
const deviceSelect = document.getElementById('input-device');
const refreshDevicesButton = document.getElementById('refresh-devices');

const MIN_DB = -60;
const MAX_DB = 0;

let audioContext;
let mediaStream;
let processorNode;
let sourceNode;
let silentGainNode;
let ws;
let wsReady = false;
let serverSampleRate = 24000;
const transcripts = new Map();
let selectedDeviceId = 'default';
let lastClientLogAt = 0;
let lastServerLogAt = 0;
let lastSendLogAt = 0;

function setStatus(message) {
  statusLabel.textContent = message;
}

function addLog(message) {
  const timestamp = new Date();
  console.log(`[log ${timestamp.toLocaleTimeString()}] ${message}`);
}

function updateMeter(element, db) {
  const clamped = Math.max(MIN_DB, Math.min(MAX_DB, Number.isFinite(db) ? db : MIN_DB));
  const percent = ((clamped - MIN_DB) / (MAX_DB - MIN_DB)) * 100;
  element.style.width = `${percent}%`;
  element.setAttribute('aria-valuenow', clamped.toFixed(1));
}

function logMetric(tag, payload) {
  const label = `[debug:${tag}]`;
  if (tag === 'client.volume' || tag === 'server.volume') {
    console.log(label, payload);
  } else {
    console.debug(label, payload);
  }
}

function calculateDb(samples) {
  if (!samples?.length) return -Infinity;
  let sumSquares = 0;
  for (let i = 0; i < samples.length; i += 1) {
    const sample = samples[i];
    sumSquares += sample * sample;
  }
  const rms = Math.sqrt(sumSquares / samples.length);
  if (rms === 0) return -Infinity;
  return 20 * Math.log10(rms);
}

function downsample(buffer, inputSampleRate, targetSampleRate) {
  if (targetSampleRate === inputSampleRate) {
    return buffer;
  }
  if (targetSampleRate > inputSampleRate) {
    console.warn('Cannot up-sample audio.');
    return null;
  }

  const ratio = inputSampleRate / targetSampleRate;
  const newLength = Math.floor(buffer.length / ratio);
  const result = new Float32Array(newLength);

  let offsetResult = 0;
  let offsetBuffer = 0;

  while (offsetResult < result.length) {
    const nextOffsetBuffer = Math.round((offsetResult + 1) * ratio);
    let accum = 0;
    let count = 0;
    for (let i = offsetBuffer; i < nextOffsetBuffer && i < buffer.length; i += 1) {
      accum += buffer[i];
      count += 1;
    }
    result[offsetResult] = accum / (count || 1);
    offsetResult += 1;
    offsetBuffer = nextOffsetBuffer;
  }

  return result;
}

function floatTo16BitPCM(float32Array) {
  const buffer = new ArrayBuffer(float32Array.length * 2);
  const view = new DataView(buffer);
  for (let i = 0; i < float32Array.length; i += 1) {
    let sample = Math.max(-1, Math.min(1, float32Array[i]));
    sample = sample < 0 ? sample * 0x8000 : sample * 0x7fff;
    view.setInt16(i * 2, sample, true);
  }
  return buffer;
}

function renderTranscripts() {
  const text = Array.from(transcripts.values()).join('\n');
  transcriptEl.textContent = text;
}

function resetState() {
  transcripts.clear();
  renderTranscripts();
  updateMeter(clientMeterFill, -Infinity);
  wsReady = false;
  serverSampleRate = 24000;
  lastClientLogAt = 0;
  lastServerLogAt = 0;
  logMetric('state.reset', { timestamp: Date.now() });
}

function applyDeviceSelection(value) {
  selectedDeviceId = value || 'default';
  if (selectedDeviceId === '') {
    selectedDeviceId = 'default';
  }
  const label = deviceSelect?.selectedOptions?.[0]?.textContent ?? 'システム既定';
  logMetric('device.selected', { deviceId: selectedDeviceId, label });
}

async function refreshDeviceList() {
  if (!deviceSelect || !navigator.mediaDevices?.enumerateDevices) {
    return;
  }

  try {
    const devices = await navigator.mediaDevices.enumerateDevices();
    const audioInputs = devices.filter((d) => d.kind === 'audioinput');
    const previousValue = deviceSelect.value;

    const keywordPreference = ['built-in', 'macbook', 'internal'];
    const secondaryPreference = ['iphone', 'ipad'];
    const scoreDevice = (device) => {
      const label = (device.label || '').toLowerCase();
      if (!label) {
        return navigator.userAgent.toLowerCase().includes('iphone') ? 5 : 2;
      }
      if (keywordPreference.some((keyword) => label.includes(keyword))) {
        return 0;
      }
      if (secondaryPreference.some((keyword) => label.includes(keyword))) {
        return 3;
      }
      if (device.deviceId === 'default') {
        return 4;
      }
      return 5;
    };

    const sortedInputs = audioInputs.slice().sort((a, b) => {
      const scoreDiff = scoreDevice(a) - scoreDevice(b);
      if (scoreDiff !== 0) return scoreDiff;
      return (a.label || '').localeCompare(b.label || '');
    });

    deviceSelect.innerHTML = '';
    const defaultOption = document.createElement('option');
    defaultOption.value = 'default';
    defaultOption.textContent = 'システム既定';
    deviceSelect.appendChild(defaultOption);

    sortedInputs.forEach((device, index) => {
      const option = document.createElement('option');
      option.value = device.deviceId || 'default';
      option.textContent = device.label || `マイク (${index + 1})`;
      deviceSelect.appendChild(option);
    });

    let candidate = 'default';
    if (sortedInputs.length > 0) {
      candidate = sortedInputs[0].deviceId || 'default';
    }
    if (selectedDeviceId && selectedDeviceId !== 'default' && sortedInputs.some((d) => d.deviceId === selectedDeviceId)) {
      candidate = selectedDeviceId;
    } else if (previousValue && previousValue !== 'default' && sortedInputs.some((d) => d.deviceId === previousValue)) {
      candidate = previousValue;
    }

    // fallback: avoid default when a specific device is available
    if (candidate === 'default' && sortedInputs.length > 0) {
      candidate = sortedInputs[0].deviceId || 'default';
    }

    deviceSelect.value = candidate;
    applyDeviceSelection(candidate);

    deviceSelect.disabled = audioInputs.length === 0;
    if (refreshDevicesButton) {
      refreshDevicesButton.disabled = false;
    }
    logMetric('device.list', {
      count: audioInputs.length,
      selected: candidate,
      labels: sortedInputs.map((d) => ({ id: d.deviceId, label: d.label })),
    });
  } catch (error) {
    console.warn('Failed to enumerate devices', error);
    addLog('デバイス一覧の取得に失敗しました');
  }
}

async function setupAudioProcessing() {
  if (!mediaStream) return;
  audioContext = audioContext ?? new (window.AudioContext || window.webkitAudioContext)();
  if (audioContext.state === 'suspended') {
    await audioContext.resume();
  }
  logMetric('audio.context', {
    state: audioContext.state,
    sampleRate: audioContext.sampleRate,
    baseLatency: audioContext.baseLatency,
  });

  sourceNode = audioContext.createMediaStreamSource(mediaStream);

  processorNode = audioContext.createScriptProcessor(4096, 1, 1);
  processorNode.onaudioprocess = (event) => {
    const channel = event.inputBuffer.getChannelData(0);
    const db = calculateDb(channel);
    updateMeter(clientMeterFill, db);
    const now = Date.now();
    const shouldLogVolume = now - lastClientLogAt >= 1000;
    if (shouldLogVolume) {
      logMetric('client.volume', {
        db: Number.isFinite(db) ? Number(db.toFixed(1)) : '-inf',
        sampleRate: audioContext.sampleRate,
        bufferLength: channel.length,
      });
      lastClientLogAt = now;
    }

    if (!wsReady || !ws || ws.readyState !== WebSocket.OPEN) {
      return;
    }

    const downsampled = downsample(channel, audioContext.sampleRate, serverSampleRate);
    if (!downsampled || downsampled.length === 0) {
      return;
    }

    const pcmBuffer = floatTo16BitPCM(downsampled);
    ws.send(pcmBuffer);
    if (now - lastSendLogAt >= 1000) {
      logMetric('client.audio.send', {
        bytes: pcmBuffer.byteLength,
        downsampledLength: downsampled.length,
        serverSampleRate,
      });
      lastSendLogAt = now;
    }
  };

  silentGainNode = audioContext.createGain();
  silentGainNode.gain.setValueAtTime(0, audioContext.currentTime);

  sourceNode.connect(processorNode);
  processorNode.connect(silentGainNode);
  silentGainNode.connect(audioContext.destination);
}

function cleanupAudioNodes() {
  if (processorNode) {
    processorNode.disconnect();
    processorNode.onaudioprocess = null;
  }
  if (silentGainNode) {
    silentGainNode.disconnect();
  }
  if (sourceNode) {
    sourceNode.disconnect();
  }
  processorNode = undefined;
  silentGainNode = undefined;
  sourceNode = undefined;
  if (mediaStream) {
    mediaStream.getTracks().forEach((track) => track.stop());
    mediaStream = undefined;
  }
  if (audioContext && audioContext.state !== 'closed') {
    audioContext.close().catch(() => {});
  }
  audioContext = undefined;
  logMetric('media.stream', { state: 'stopped' });
  logMetric('audio.context', { state: 'closed' });
}

function closeWebSocket() {
  if (ws) {
    try {
      if (ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ type: 'control', action: 'stop' }));
      }
      ws.close();
    } catch (error) {
      console.warn('Failed to close websocket cleanly', error);
    }
  }
  ws = undefined;
  wsReady = false;
  lastServerLogAt = 0;
  logMetric('ws.state', { state: 'closed' });
}

function createWebSocket() {
  const protocol = window.location.protocol === 'https:' ? 'wss' : 'ws';
  const url = `${protocol}://${window.location.host}/realtime`;
  const socket = new WebSocket(url);
  socket.binaryType = 'arraybuffer';
  addLog('WebSocket接続を開始しました');
  logMetric('ws.state', { state: 'connecting', url });

  socket.addEventListener('open', () => {
    wsReady = true;
    setStatus('録音中');
    addLog('WebSocket接続が確立しました');
    stopButton.disabled = false;
    logMetric('ws.state', { state: 'open' });
  });

  socket.addEventListener('close', (event) => {
    wsReady = false;
    addLog(`WebSocket切断: code=${event.code}`);
    setStatus('切断');
    stopButton.disabled = true;
    startButton.disabled = false;
    logMetric('ws.state', { state: 'closed', code: event.code, reason: event.reason });
  });

  socket.addEventListener('error', (event) => {
    console.error('WebSocket error', event);
    addLog('WebSocketエラーが発生しました');
    logMetric('ws.state', { state: 'error' });
  });

  socket.addEventListener('message', (event) => {
    if (typeof event.data === 'string') {
      try {
        const payload = JSON.parse(event.data);
        handleServerEvent(payload);
      } catch (error) {
        console.warn('Invalid JSON from server', error);
      }
    }
  });

  return socket;
}

function handleServerEvent(event) {
  switch (event.type) {
    case 'ready':
      if (event.sampleRate) {
        serverSampleRate = event.sampleRate;
        addLog(`サーバー準備完了 (sampleRate=${event.sampleRate}Hz)`);
      }
      break;
    case 'transcript':
      transcripts.set(event.itemId, event.text);
      renderTranscripts();
      break;
    case 'transcript-delta': {
      const previous = transcripts.get(event.itemId) ?? '';
      transcripts.set(event.itemId, previous + event.text);
      renderTranscripts();
      break;
    }
    case 'response-text-delta': {
      const key = `response-${event.responseId}`;
      const previous = transcripts.get(key) ?? '';
      transcripts.set(key, previous + event.text);
      renderTranscripts();
      break;
    }
    case 'response-text-done':
      addLog('レスポンスのテキスト出力完了');
      break;
    case 'volume': {
        const now = Date.now();
        if (now - lastServerLogAt >= 1000) {
          const value = Number.isFinite(event.db)
            ? Number(event.db.toFixed ? event.db.toFixed(1) : event.db)
            : '-inf';
          logMetric('server.volume', { db: value, timestamp: now });
          lastServerLogAt = now;
        }
      }
      break;
    case 'error':
      addLog(`サーバーエラー: ${event.message}`);
      console.error('Server error', event);
      break;
    default:
      addLog(`未処理イベント: ${event.type}`);
  }
}

async function startRecording() {
  if (!navigator.mediaDevices?.getUserMedia) {
    setStatus('このブラウザはマイク入力に対応していません');
    return;
  }

  resetState();
  startButton.disabled = true;
  stopButton.disabled = true;

  try {
    setStatus('マイクアクセス中...');
    const audioConstraints = {
      channelCount: 1,
      echoCancellation: false,
      noiseSuppression: false,
      autoGainControl: false,
    };

    if (selectedDeviceId && selectedDeviceId !== 'default') {
      audioConstraints.deviceId = { exact: selectedDeviceId };
    }

    mediaStream = await navigator.mediaDevices.getUserMedia({
      audio: audioConstraints,
    });
    logMetric('media.stream', { state: 'acquired', deviceId: selectedDeviceId });

    // 権限付与後にデバイスリストを更新
    await refreshDeviceList();

    const [track] = mediaStream.getAudioTracks();
    const settings = track?.getSettings?.();
    if (settings?.deviceId) {
      applyDeviceSelection(settings.deviceId);
      if (deviceSelect) {
        deviceSelect.value = settings.deviceId;
      }
    }

    setStatus('オーディオ処理を初期化中...');
    await setupAudioProcessing();

    setStatus('サーバー接続中...');
    ws = createWebSocket();
  } catch (error) {
    console.error('Failed to start recording', error);
    addLog(`開始に失敗: ${error.message}`);
    setStatus('開始に失敗しました');
    cleanupAudioNodes();
    closeWebSocket();
    startButton.disabled = false;
    logMetric('media.stream', { state: 'error', error: error.message });
    return;
  }
}

function stopRecording() {
  closeWebSocket();
  cleanupAudioNodes();
  setStatus('停止しました');
  stopButton.disabled = true;
  startButton.disabled = false;
  addLog('録音を停止しました');
}

startButton.addEventListener('click', startRecording);
stopButton.addEventListener('click', stopRecording);

if (deviceSelect) {
  deviceSelect.addEventListener('change', (event) => {
    applyDeviceSelection(event.target.value);
    addLog(`使用デバイスを変更: ${deviceSelect.selectedOptions[0]?.textContent ?? 'システム既定'}`);
  });
}

if (refreshDevicesButton) {
  refreshDevicesButton.addEventListener('click', async () => {
    addLog('デバイス一覧を再取得します');
    try {
      // iOS Safariなどでは権限なしだとlabelが空のため、軽くアクセス
      const tempStream = await navigator.mediaDevices?.getUserMedia?.({ audio: true });
      tempStream?.getTracks?.().forEach((track) => track.stop());
    } catch (error) {
      console.warn('Permission request for refresh failed', error);
    }
    await refreshDeviceList();
  });
}

refreshDeviceList();

window.addEventListener('beforeunload', () => {
  closeWebSocket();
  if (audioContext && audioContext.state !== 'closed') {
    audioContext.close().catch(() => {});
  }
});
