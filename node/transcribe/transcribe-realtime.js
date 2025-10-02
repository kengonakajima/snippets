// rt-stt.js
import "dotenv/config";
import WebSocket from "ws";
import fs from "node:fs";
import { spawn, spawnSync } from "node:child_process";

const FALLBACK_TIMEOUT_MS = Number(process.env.REALTIME_TIMEOUT_MS ?? "10000");
const GRACE_EXTRA_MS = Number(process.env.REALTIME_GRACE_MS ?? "10000");
const BYTES_PER_MS = 32; // 16kHz * 2 bytes / 1000ms
const MAX_QUEUE_LENGTH = Number(process.env.REALTIME_MAX_QUEUE ?? "50");

const startTime = Date.now();

const log = (message, extra = {}) => {
  const elapsed = ((Date.now() - startTime) / 1000).toFixed(3);
  console.log(JSON.stringify({ t: Number(elapsed), message, ...extra }));
};

// 例としてWAV音声をPCM16に変換してストリーミング送信
const AUDIO_FILE = process.argv[2] ?? "./japanese_sample_24k.wav";

if (!fs.existsSync(AUDIO_FILE)) {
  throw new Error(`Audio file not found: ${AUDIO_FILE}`);
}

function readAudioDurationMs(path) {
  const result = spawnSync("ffprobe", [
    "-v",
    "error",
    "-show_entries",
    "format=duration",
    "-of",
    "default=noprint_wrappers=1:nokey=1",
    path
  ], { encoding: "utf8" });

  if (result.status === 0) {
    const secs = parseFloat(result.stdout.trim());
    if (!Number.isNaN(secs) && Number.isFinite(secs) && secs > 0) {
      return Math.round(secs * 1000);
    }
  }

  if (result.status !== 0) {
    log("ffprobe_failed", { status: result.status, stderr: result.stderr?.toString?.() ?? "" });
  }
  return undefined;
}

const audioDurationMs = readAudioDurationMs(AUDIO_FILE);
const timeoutLimitMs = audioDurationMs ? audioDurationMs + GRACE_EXTRA_MS : FALLBACK_TIMEOUT_MS;
log("timeout_config", { audioDurationMs, timeoutLimitMs });

const url = "wss://api.openai.com/v1/realtime?model=gpt-4o-realtime-preview";
const ws = new WebSocket(url, {
  headers: {
    Authorization: `Bearer ${process.env.OPENAI_API_KEY}`,
    "OpenAI-Beta": "realtime=v1"
  }
});

let ffmpeg;
let timeoutId;
let receivedResult = false;
let shuttingDown = false;
let chunkCounter = 0;
let ffmpegCompleted = false;
let commitSent = false;

const chunkQueue = [];
let sendingQueue = false;

const resumeFfmpegIfPaused = () => {
  if (ffmpeg?.stdout?.isPaused()) {
    ffmpeg.stdout.resume();
  }
};

const pendingItems = new Set();
const transcripts = new Map();
const itemOrder = [];

const clearTimeoutIfNeeded = () => {
  if (timeoutId) {
    clearTimeout(timeoutId);
    timeoutId = null;
  }
};

const markResult = () => {
  if (!receivedResult) {
    receivedResult = true;
    clearTimeoutIfNeeded();
  }
};

const stopSession = (exitCode, details) => {
  if (shuttingDown) {
    return;
  }
  shuttingDown = true;
  clearTimeoutIfNeeded();
  if (ffmpeg) {
    ffmpeg.kill("SIGKILL");
    ffmpeg = undefined;
  }
  const isOpen = ws.readyState === WebSocket.OPEN || ws.readyState === WebSocket.CONNECTING || ws.readyState === WebSocket.CLOSING;
  if (isOpen) {
    ws.terminate();
  }
  if (details) {
    log("session_closed", details);
  }
  console.log("closed");
  process.exit(exitCode);
};

const tryFinish = () => {
  if (!ffmpegCompleted) {
    return;
  }
  if (!receivedResult) {
    return;
  }
  if (pendingItems.size > 0) {
    return;
  }
  const orderedTranscripts = itemOrder
    .map((itemId) => ({ itemId, transcript: transcripts.get(itemId) ?? "" }))
    .filter((entry) => transcripts.has(entry.itemId));
  log("all_transcriptions_complete", { count: orderedTranscripts.length, transcripts: orderedTranscripts });
  stopSession(0, { transcripts: orderedTranscripts });
};

const analyzePcmChunk = (buffer) => {
  if (!buffer || buffer.length === 0) {
    return { sampleCount: 0, rms: 0, rmsDb: -Infinity, peak: 0 };
  }
  const sampleCount = buffer.length / 2;
  let sumSquares = 0;
  let peak = 0;
  for (let i = 0; i < buffer.length; i += 2) {
    const sample = buffer.readInt16LE(i);
    sumSquares += sample * sample;
    const absSample = Math.abs(sample);
    if (absSample > peak) {
      peak = absSample;
    }
  }
  const rms = Math.sqrt(sumSquares / sampleCount);
  const norm = rms / 32768;
  const rmsDb = norm > 0 ? 20 * Math.log10(norm) : -Infinity;
  return {
    sampleCount,
    rms: Number(rms.toFixed(2)),
    rmsDb: Number(rmsDb.toFixed(2)),
    peak
  };
};

const enqueueChunk = (chunk) => {
  const stats = analyzePcmChunk(chunk);
  const durationMs = chunk.length / BYTES_PER_MS;
  chunkQueue.push({ chunk, stats, durationMs });
  if (chunkQueue.length >= MAX_QUEUE_LENGTH && ffmpeg?.stdout && !ffmpeg.stdout.isPaused()) {
    ffmpeg.stdout.pause();
    log("queue_paused_ffmpeg", { queueLength: chunkQueue.length });
  }
  if (!sendingQueue) {
    processQueue();
  }
};

const processQueue = () => {
  if (chunkQueue.length === 0) {
    sendingQueue = false;
    resumeFfmpegIfPaused();
    maybeFinalizeStream();
    return;
  }
  sendingQueue = true;
  const { chunk, stats, durationMs } = chunkQueue.shift();
  chunkCounter += 1;
  log("send_chunk", { chunkIndex: chunkCounter, bytes: chunk.length, ...stats });
  const payload = JSON.stringify({ type: "input_audio_buffer.append", audio: chunk.toString("base64") });
  if (ws.readyState !== WebSocket.OPEN) {
    log("ws_not_open_drop_chunk", { chunkIndex: chunkCounter });
    return;
  }

  ws.send(payload, (err) => {
    if (err) {
      log("ws_send_error", { error: err.message, chunkIndex: chunkCounter });
      stopSession(1, { error: err.message });
      return;
    }
    const delay = Math.max(0, Math.round(durationMs));
    setTimeout(processQueue, delay);
  });
};

const maybeFinalizeStream = () => {
  if (!ffmpegCompleted || commitSent) {
    return;
  }
  if (sendingQueue || chunkQueue.length > 0) {
    return;
  }
  commitSent = true;
  log("audio_stream_drained", { chunks: chunkCounter });
  ws.send(JSON.stringify({ type: "input_audio_buffer.commit" }));
  ws.send(JSON.stringify({ type: "response.create", response: { modalities: ["text"] } }));
  tryFinish();
};

ws.on("open", () => {
  // セッション設定：gpt-4o-transcribe を STT に指定
  ws.send(JSON.stringify({
    type: "session.update",
    session: {
      modalities: ["audio", "text"],
      input_audio_format: "pcm16",
      input_audio_transcription: { model: "gpt-4o-transcribe", language: "ja" },
      turn_detection: {
        type: "server_vad",
        silence_duration_ms: 300,
        create_response: false,
        interrupt_response: false
      },
      temperature: Number(process.env.REALTIME_TEMPERATURE ?? "0.6")
    }
  }));

  // ffmpegでWAVを16kHz mono PCM16に変換しながらチャンク送信
  ffmpeg = spawn("ffmpeg", [
    "-hide_banner",
    "-loglevel",
    "error",
    "-i",
    AUDIO_FILE,
    "-ac",
    "1",
    "-ar",
    "16000",
    "-f",
    "s16le",
    "-"
  ]);

  ffmpeg.stdout.on("data", (chunk) => {
    enqueueChunk(chunk);
  });

  ffmpeg.on("close", (code) => {
    if (code !== 0) {
      log("ffmpeg_exit_error", { code });
      stopSession(1, { code });
      return;
    }
    ffmpegCompleted = true;
    log("ffmpeg_completed", { chunks: chunkCounter });
    maybeFinalizeStream();
  });

  ffmpeg.stderr.on("data", (data) => {
    log("ffmpeg_stderr", { message: data.toString() });
  });

  ffmpeg.on("error", (err) => {
    log("ffmpeg_spawn_error", { error: err.message });
    stopSession(1, { error: err.message });
  });

  timeoutId = setTimeout(() => {
    log("timeout", { limitMs: timeoutLimitMs, chunksSent: chunkCounter, receivedResult });
    stopSession(1, { reason: "timeout" });
  }, timeoutLimitMs);
});

ws.on("message", (data) => {
  const msg = JSON.parse(data.toString());
  log("ws_message", { payload: msg });
  if (msg.type === "conversation.item.created") {
    const hasInputAudio = Array.isArray(msg.item?.content) && msg.item.content.some((part) => part.type === "input_audio");
    if (hasInputAudio) {
      const id = msg.item.id;
      if (!pendingItems.has(id)) {
        pendingItems.add(id);
        itemOrder.push(id);
      }
    }
  }
  // 文字起こしが完了すると、以下のようなイベントが届く（実装側で拾って表示）
  if (msg.type?.includes("input_audio_transcription")) {
    markResult();
    console.log("STT event:", msg.type, msg);
  }
  if (msg.type === "response.output_text.delta") {
    markResult();
    log("transcript_delta", { delta: msg.delta });
  }
  if (
    msg.type === "conversation.item.input_audio_transcription.delta" ||
    msg.type === "conversation.item.input_audio_transcription.completed"
  ) {
    markResult();
    const id = msg.item_id;
    const prev = transcripts.get(id) ?? "";
    if (msg.delta) {
      transcripts.set(id, prev + msg.delta);
    }
  }
  if (msg.type === "conversation.item.input_audio_transcription.completed") {
    const id = msg.item_id;
    if (msg.transcript) {
      transcripts.set(id, msg.transcript);
    }
    pendingItems.delete(id);
    log("transcription_completed", { transcript: msg.transcript, itemId: msg.item_id });
    tryFinish();
  }
  if (msg.type === "input_audio_buffer.committed" && typeof msg.item_id === "string") {
    // ensure items recognized even if no transcription (e.g., silence)
    if (!pendingItems.has(msg.item_id) && !transcripts.has(msg.item_id)) {
      transcripts.set(msg.item_id, "");
    }
    tryFinish();
  }
});

ws.on("error", (err) => {
  log("ws_error", { error: err.message });
  stopSession(1);
});

ws.on("close", () => {
  if (!shuttingDown) {
    stopSession(receivedResult ? 0 : 1);
  }
});
