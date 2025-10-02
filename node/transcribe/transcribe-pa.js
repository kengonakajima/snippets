// transcribe-pa.js
import "dotenv/config";
import WebSocket from "ws";
import process from "node:process";
import { setTimeout, clearTimeout, setInterval, clearInterval } from "node:timers";
import { createRequire } from "module";

const require = createRequire(import.meta.url);

if (process.platform !== "darwin") {
  console.error("This script requires macOS with PAmac.node (PortAudio binding).");
  process.exit(1);
}

const PortAudio = require("./PAmac.node");

const SAMPLE_RATE = Number(process.env.PA_SAMPLE_RATE ?? "16000");
const BLOCK_SIZE = Number(process.env.PA_BLOCK_SIZE ?? "64");
const CHUNK_MS = Number(process.env.PA_CHUNK_MS ?? "100");
const CHUNK_BYTES = Math.max(2, Math.floor((SAMPLE_RATE * 2 * CHUNK_MS) / 1000));
const POLL_INTERVAL_MS = Number(process.env.PA_POLL_INTERVAL_MS ?? "20");
const COMMIT_INTERVAL_MS = Number(process.env.PA_COMMIT_INTERVAL_MS ?? "2000");
const SESSION_TIMEOUT_MS = Number(process.env.PA_SESSION_TIMEOUT_MS ?? "300000");
const TOTAL_DURATION_MS = Number(process.env.PA_TOTAL_DURATION_MS ?? "0");
const RESPONSE_MODALITIES = process.env.PA_RESPONSE_MODALITIES?.split(",").map((m) => m.trim()).filter(Boolean) ?? ["text"];

const startTime = Date.now();
const log = (message, extra = {}) => {
  const elapsed = ((Date.now() - startTime) / 1000).toFixed(3);
  console.error(JSON.stringify({ t: Number(elapsed), message, ...extra }));
};

const micBuffers = [];
let bufferedBytes = 0;
let chunkCounter = 0;
let audioSinceLastCommit = false;
let lastCommitAt = Date.now();
let lastActivityAt = Date.now();
let shuttingDown = false;

let pollTimer = null;
let commitTimer = null;
let sessionTimer = null;
let runLimitTimer = null;

const pendingItems = new Set();
const transcripts = new Map();
const itemOrder = [];

const toInt16Array = (data) => {
  if (!data) return new Int16Array(0);
  if (data instanceof Int16Array) return data;
  if (ArrayBuffer.isView(data)) {
    return new Int16Array(data.buffer, data.byteOffset, Math.floor(data.byteLength / 2));
  }
  if (Buffer.isBuffer(data)) {
    return new Int16Array(data.buffer, data.byteOffset, Math.floor(data.byteLength / 2));
  }
  if (Array.isArray(data)) {
    const out = new Int16Array(data.length);
    for (let i = 0; i < data.length; i++) out[i] = data[i] | 0;
    return out;
  }
  return new Int16Array(0);
};


const refreshSessionTimeout = () => {
  if (sessionTimer) clearTimeout(sessionTimer);
  sessionTimer = setTimeout(() => {
    log("session_timeout", { timeoutMs: SESSION_TIMEOUT_MS });
    shutdown(1);
  }, SESSION_TIMEOUT_MS);
};

const appendMicSamples = (int16) => {
  if (!int16 || int16.length === 0) return;
  const buf = Buffer.from(int16.buffer, int16.byteOffset, int16.length * 2);
  micBuffers.push(Buffer.from(buf));
  bufferedBytes += buf.length;
  sendAvailableChunks();
};

const sendAvailableChunks = () => {
  if (ws.readyState !== WebSocket.OPEN) return;
  while (bufferedBytes >= CHUNK_BYTES) {
    const chunk = Buffer.alloc(CHUNK_BYTES);
    let offset = 0;
    while (offset < CHUNK_BYTES && micBuffers.length > 0) {
      const next = micBuffers[0];
      const copyLen = Math.min(next.length, CHUNK_BYTES - offset);
      next.copy(chunk, offset, 0, copyLen);
      if (copyLen === next.length) {
        micBuffers.shift();
      } else {
        micBuffers[0] = next.slice(copyLen);
      }
      offset += copyLen;
      bufferedBytes -= copyLen;
    }
    if (offset === CHUNK_BYTES) {
      sendChunk(chunk);
    } else {
      // Incomplete chunk; push back remainder
      if (chunk.length > 0) {
        micBuffers.unshift(chunk.slice(0, offset));
        bufferedBytes += offset;
      }
      break;
    }
  }
};

const sendChunk = (chunk) => {
  chunkCounter += 1;
  const payload = JSON.stringify({ type: "input_audio_buffer.append", audio: chunk.toString("base64") });
  ws.send(payload, (err) => {
    if (err) {
      log("ws_send_error", { error: err.message, chunkIndex: chunkCounter });
      shutdown(1);
    }
  });
  audioSinceLastCommit = true;
  lastActivityAt = Date.now();
  refreshSessionTimeout();
};

const flushRemaining = () => {
  if (ws.readyState !== WebSocket.OPEN) return;
  if (bufferedBytes === 0) return;
  const chunk = Buffer.alloc(bufferedBytes);
  let offset = 0;
  while (micBuffers.length > 0) {
    const next = micBuffers.shift();
    next.copy(chunk, offset);
    offset += next.length;
  }
  bufferedBytes = 0;
  if (offset > 0) {
    sendChunk(chunk.slice(0, offset));
  }
};

const commitAudioBuffer = (force = false) => {
  if (ws.readyState !== WebSocket.OPEN) return;
  const now = Date.now();
  if (!force) {
    if (!audioSinceLastCommit) return;
    if (now - lastCommitAt < COMMIT_INTERVAL_MS) return;
  }
  ws.send(JSON.stringify({ type: "input_audio_buffer.commit" }));
  ws.send(JSON.stringify({ type: "response.create", response: { modalities: RESPONSE_MODALITIES } }));
  log("commit_sent", { chunks: chunkCounter, force });
  audioSinceLastCommit = false;
  lastCommitAt = now;
  refreshSessionTimeout();
};

const handleConversationItemCreated = (msg) => {
  const hasAudio = Array.isArray(msg.item?.content) && msg.item.content.some((part) => part.type === "input_audio");
  if (!hasAudio) return;
  const id = msg.item.id;
  if (!pendingItems.has(id)) {
    pendingItems.add(id);
    itemOrder.push(id);
  }
};

const handleTranscriptionDelta = (msg) => {
  const prev = transcripts.get(msg.item_id) ?? "";
  if (msg.delta) {
    transcripts.set(msg.item_id, prev + msg.delta);
  }
};

const handleTranscriptionCompleted = (msg) => {
  const id = msg.item_id;
  if (msg.transcript) {
    transcripts.set(id, msg.transcript);
  }
  pendingItems.delete(id);
  const transcript = transcripts.get(id) ?? "";
  log("transcription_completed", { itemId: id, transcript });
  process.stdout.write(`> ${transcript}\n`);
  refreshSessionTimeout();
};

const handleWsMessage = (data) => {
  const msg = JSON.parse(data.toString());
  log("ws_message", { type: msg.type });
  if (msg.type === "conversation.item.created") {
    handleConversationItemCreated(msg);
  }
  if (msg.type === "conversation.item.input_audio_transcription.delta") {
    handleTranscriptionDelta(msg);
  }
  if (msg.type === "conversation.item.input_audio_transcription.completed") {
    handleTranscriptionCompleted(msg);
  }
};

const fetchMicSamples = () => {
  const rec = PortAudio.getRecordedSamples();
  const data = toInt16Array(rec);
  if (data.length === 0) return;
  appendMicSamples(data);
  if (typeof PortAudio.discardRecordedSamples === "function") {
    PortAudio.discardRecordedSamples(data.length);
  }
};

const shutdown = (exitCode = 0) => {
  if (shuttingDown) return;
  shuttingDown = true;
  log("shutdown", { exitCode });
  if (pollTimer) clearInterval(pollTimer);
  if (commitTimer) clearInterval(commitTimer);
  if (sessionTimer) clearTimeout(sessionTimer);
  if (runLimitTimer) clearTimeout(runLimitTimer);
  if (ws.readyState === WebSocket.OPEN) {
    flushRemaining();
    commitAudioBuffer(true);
  }
  if (PortAudio && typeof PortAudio.stopMic === "function") {
    PortAudio.stopMic();
  }
  if (ws.readyState === WebSocket.CLOSED || ws.readyState === WebSocket.CLOSING) {
    process.exit(exitCode);
  } else {
    ws.once("close", () => process.exit(exitCode));
    ws.close();
    setTimeout(() => process.exit(exitCode), 500).unref();
  }
};

process.on("SIGINT", () => {
  log("signal", { type: "SIGINT" });
  shutdown(0);
});

process.on("SIGTERM", () => {
  log("signal", { type: "SIGTERM" });
  shutdown(0);
});

const url = "wss://api.openai.com/v1/realtime?model=gpt-4o-realtime-preview";
const ws = new WebSocket(url, {
  headers: {
    Authorization: `Bearer ${process.env.OPENAI_API_KEY}`,
    "OpenAI-Beta": "realtime=v1"
  }
});

ws.on("open", () => {
  log("ws_open", {});
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

  PortAudio.initSampleBuffers(SAMPLE_RATE, SAMPLE_RATE, BLOCK_SIZE);
  PortAudio.startMic();
  log("mic_started", { sampleRate: SAMPLE_RATE, blockSize: BLOCK_SIZE, chunkMs: CHUNK_MS });

  refreshSessionTimeout();

  pollTimer = setInterval(() => {
    fetchMicSamples();
    sendAvailableChunks();
    commitAudioBuffer(false);
  }, Math.max(1, POLL_INTERVAL_MS));

  commitTimer = setInterval(() => {
    commitAudioBuffer(false);
  }, Math.max(100, Math.floor(COMMIT_INTERVAL_MS / 2)));

  if (TOTAL_DURATION_MS > 0) {
    runLimitTimer = setTimeout(() => {
      log("run_limit_reached", { limitMs: TOTAL_DURATION_MS });
      shutdown(0);
    }, TOTAL_DURATION_MS);
  }
});

ws.on("message", handleWsMessage);

ws.on("error", (err) => {
  log("ws_error", { error: err.message });
  shutdown(1);
});

ws.on("close", (code, reason) => {
  log("ws_close", { code, reason: reason?.toString?.() ?? "" });
  shutdown(0);
});
