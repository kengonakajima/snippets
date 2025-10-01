import path from 'path';
import dotenv from 'dotenv';
import express from 'express';
import { createServer } from 'http';
import { WebSocketServer } from 'ws';
import { createRealtimeConnection, OutboundMessage } from './openaiRealtime';
import { ClientMessage, ServerMessage } from './types';

dotenv.config();

const port = Number(process.env.PORT ?? 3000);
const workspaceDirEnv = process.env.WORKSPACE_DIR ?? './workspace';
const workspaceDir = path.resolve(process.cwd(), workspaceDirEnv);
const apiKey = process.env.OPENAI_API_KEY;
const model = process.env.OPENAI_REALTIME_MODEL ?? 'gpt-4o-mini-realtime-preview-2024-12-17';

if (!apiKey) {
  console.warn('[server] OPENAI_API_KEY is not set. OpenAI integration will be disabled.');
}

const app = express();

app.get('/health', (_req, res) => {
  res.json({ status: 'ok' });
});

const server = createServer(app);

const wss = new WebSocketServer({ server, path: '/ws' });

wss.on('connection', (socket, request) => {
  console.log('[ws] connection from %s', request.socket.remoteAddress ?? 'unknown');

  const sendStatus = (body: string) => {
    const payload: ServerMessage = { type: 'status', body };
    socket.send(JSON.stringify(payload));
  };

  if (!apiKey) {
    sendStatus('Server missing OPENAI_API_KEY. Close connection.');
    socket.close(1011, 'missing OPENAI_API_KEY');
    return;
  }

  let openaiReady = false;
  const pending: OutboundMessage[] = [];
  let realtime = createRealtimeConnection({
    apiKey,
    model,
    onOpen: () => {
      openaiReady = true;
      console.log('[openai] connection established');
      sendStatus('OpenAI connection established');
      flushPending();
    },
    onMessage: (data) => {
      const text = typeof data === 'string' ? data : data.toString();
      console.log('[openai] -> client', text);
      const payload: ServerMessage = {
        type: 'openai-event',
        body: tryParseJson(text) ?? text,
      };
      socket.send(JSON.stringify(payload));
    },
    onClose: (code, reason) => {
      console.log('[openai] connection closed code=%d reason=%s', code, reason.toString());
      socket.close(code, reason.toString());
    },
    onError: (error) => {
      console.error('[openai] error', error);
      sendStatus(`OpenAI error: ${error.message}`);
    },
  });

  function flushPending() {
    if (!openaiReady || pending.length === 0) {
      return;
    }
    while (pending.length > 0) {
      const item = pending.shift();
      if (!item) {
        continue;
      }
      const sent = realtime.send(item);
      if (!sent) {
        pending.unshift(item);
        break;
      }
    }
  }

  function sendToOpenAI(message: OutboundMessage) {
    if (!openaiReady) {
      pending.push(message);
      return false;
    }
    const sent = realtime.send(message);
    if (!sent) {
      pending.push(message);
    }
    return sent;
  }

  socket.on('message', (data) => {
    const text = data.toString();
    console.log('[client] -> server', text);
    const parsed = tryParseJson<ClientMessage>(text);

    if (parsed && parsed.type === 'user-message') {
      const conversationItem = JSON.stringify({
        type: 'conversation.item.create',
        item: {
          type: 'message',
          role: 'user',
          content: [
            {
              type: 'input_text',
              text: parsed.body,
            },
          ],
        },
      });

      const responseCreate = JSON.stringify({
        type: 'response.create',
        response: {
          modalities: ['text'],
        },
      });

      sendToOpenAI(conversationItem);
      sendToOpenAI(responseCreate);
      return;
    }

    sendToOpenAI(data);
  });

  socket.on('close', (code, reason) => {
    console.log('[ws] client disconnected code=%d reason=%s', code, reason.toString());
    realtime.close(code, reason.toString());
  });

  socket.on('error', (error) => {
    console.error('[ws] error', error);
  });
});

server.listen(port, () => {
  console.log('[server] listening on port %d', port);
  console.log('[server] workspace directory: %s', workspaceDir);
  console.log('[server] realtime model: %s', model);
});

function tryParseJson<T>(value: string): T | null {
  try {
    return JSON.parse(value) as T;
  } catch (error) {
    return null;
  }
}
