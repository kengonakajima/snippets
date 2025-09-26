import 'dotenv/config';
import express from 'express';
import http from 'http';
import path from 'path';
import { fileURLToPath } from 'url';
import { WebSocketServer, WebSocket } from 'ws';
import OpenAI from 'openai';
import { OpenAIRealtimeWS } from 'openai/realtime/ws';

const REQUIRED_ENV = ['OPENAI_API_KEY'];
const missing = REQUIRED_ENV.filter((key) => !process.env[key] || process.env[key].length === 0);
if (missing.length > 0) {
  console.error(`Missing environment variables: ${missing.join(', ')}`);
  console.error('Set them in a .env file or export them before starting the server.');
  process.exit(1);
}

const MODEL = process.env.OPENAI_REALTIME_MODEL ?? 'gpt-4o-realtime-preview';
const PORT = Number(process.env.PORT ?? 3000);
const SAMPLE_RATE = 24000;

const openai = new OpenAI({ apiKey: process.env.OPENAI_API_KEY });

const app = express();

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const publicPath = path.resolve(__dirname, '../public');
app.use(express.static(publicPath));

app.get('/health', (_req, res) => {
  res.json({ status: 'ok', model: MODEL });
});

const server = http.createServer(app);
const wss = new WebSocketServer({ server, path: '/realtime' });

function calculateDecibels(buffer) {
  if (!buffer || buffer.length < 2) return -Infinity;
  const samples = Math.floor(buffer.length / 2);
  if (samples === 0) return -Infinity;

  let sumSquares = 0;
  for (let i = 0; i < samples; i += 1) {
    const sample = buffer.readInt16LE(i * 2);
    sumSquares += sample * sample;
  }

  const rms = Math.sqrt(sumSquares / samples);
  if (rms === 0) return -Infinity;

  const db = 20 * Math.log10(rms / 32768);
  return Number.isFinite(db) ? db : -Infinity;
}

wss.on('connection', async (socket) => {
  console.log('Client connected');

  let realtime;
  let audioChunks = [];
  let lastVolumeSentAt = Date.now();
  let hasPendingAudio = false;

  try {
    realtime = await OpenAIRealtimeWS.create(openai, { model: MODEL });
  } catch (error) {
    console.error('Failed to open realtime session:', error);
    socket.send(
      JSON.stringify({
        type: 'error',
        message: 'Failed to connect to OpenAI Realtime API.',
        detail: error?.message ?? 'Unknown error',
      }),
    );
    socket.close(1011, 'Realtime session error');
    return;
  }

  const pendingEvents = [];
  let realtimeReady = realtime.socket.readyState === realtime.socket.OPEN;

  const sendRealtimeNow = (event) => {
    try {
      realtime.send(event);
    } catch (error) {
      console.error('Failed to send realtime event:', error);
    }
  };

  const flushPending = () => {
    if (!realtimeReady || realtime.socket.readyState !== realtime.socket.OPEN) {
      return;
    }
    while (pendingEvents.length > 0) {
      const event = pendingEvents.shift();
      sendRealtimeNow(event);
    }
  };

  const enqueueRealtime = (event) => {
    if (realtimeReady && realtime.socket.readyState === realtime.socket.OPEN) {
      sendRealtimeNow(event);
    } else {
      pendingEvents.push(event);
    }
  };

  if (!realtimeReady) {
    realtime.socket.once('open', () => {
      realtimeReady = true;
      flushPending();
    });
  }

  realtime.socket.once('close', () => {
    realtimeReady = false;
    pendingEvents.length = 0;
  });

  const commitIfNeeded = () => {
    if (!hasPendingAudio) return;
    enqueueRealtime({ type: 'input_audio_buffer.commit' });
    hasPendingAudio = false;
  };

  realtime.on('event', (event) => {
    if (event.type === 'conversation.item.input_audio_transcription.completed') {
      socket.send(
        JSON.stringify({
          type: 'transcript',
          text: event.transcript,
          itemId: event.item_id,
        }),
      );
    } else if (event.type === 'conversation.item.input_audio_transcription.delta' && event.delta) {
      socket.send(
        JSON.stringify({
          type: 'transcript-delta',
          text: event.delta,
          itemId: event.item_id,
        }),
      );
    } else if (event.type === 'error') {
      socket.send(JSON.stringify({ type: 'error', message: event.error?.message ?? 'Realtime API error' }));
    }
  });

  realtime.on('error', (error) => {
    console.error('Realtime error:', error);
    socket.send(
      JSON.stringify({
        type: 'error',
        message: 'Realtime API error',
        detail: error?.message ?? 'Unknown error',
      }),
    );
  });

  enqueueRealtime({
    type: 'session.update',
    session: {
      modalities: ['audio', 'text'],
      instructions:
        'You are a Japanese transcription assistant. Only return faithful transcripts of the incoming speech in Japanese.',
      voice: 'alloy',
      input_audio_format: 'pcm16',
      output_audio_format: 'pcm16',
      input_audio_transcription: {
        model: 'gpt-4o-transcribe',
      },
      turn_detection: {
        type: 'server_vad',
        threshold: 0.5,
        prefix_padding_ms: 300,
        silence_duration_ms: 500,
      },
    },
  });

  socket.send(JSON.stringify({ type: 'ready', sampleRate: SAMPLE_RATE }));

  socket.on('message', (data) => {
    if (typeof data === 'string') {
      try {
        const message = JSON.parse(data);
        if (message?.type === 'control' && message.action === 'commit') {
          commitIfNeeded();
        } else if (message?.type === 'control' && message.action === 'stop') {
          commitIfNeeded();
        }
      } catch (error) {
        console.warn('Failed to parse message from client:', error);
      }
      return;
    }

    if (!Buffer.isBuffer(data)) {
      data = Buffer.from(data);
    }

    audioChunks.push(data);
    if (data.length > 0) {
      hasPendingAudio = true;
    }

    enqueueRealtime({
      type: 'input_audio_buffer.append',
      audio: data.toString('base64'),
    });

    const now = Date.now();
    if (now - lastVolumeSentAt >= 1000) {
      const combined = Buffer.concat(audioChunks);
      audioChunks = [];
      const db = calculateDecibels(combined);
      socket.send(
        JSON.stringify({
          type: 'volume',
          db: Number.isFinite(db) ? Number(db.toFixed(1)) : -Infinity,
        }),
      );
      console.log(`Volume: ${Number.isFinite(db) ? db.toFixed(1) : '-inf'} dB`);
      // Realtime APIのサーバーVADに委ねるため、ここではコミットしない
      lastVolumeSentAt = now;
    }
  });

  socket.on('close', () => {
    console.log('Client disconnected');
    try {
      commitIfNeeded();
      const state = realtime.socket.readyState;
      if (state === WebSocket.OPEN || state === WebSocket.CLOSING) {
        realtime.close();
      } else if (state === WebSocket.CONNECTING) {
        realtime.socket.terminate();
      }
    } catch (error) {
      console.error('Error closing realtime session:', error);
    }
  });

  socket.on('error', (error) => {
    console.error('Client WebSocket error:', error);
  });
});

server.listen(PORT, () => {
  console.log(`Server listening on http://0.0.0.0:${PORT}`);
});
