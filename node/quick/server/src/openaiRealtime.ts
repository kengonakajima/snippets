import { TextDecoder, TextEncoder } from 'util';
import WebSocket, { RawData } from 'ws';

export type RealtimeConnectionConfig = {
  apiKey: string;
  model: string;
  onOpen?: () => void;
  onMessage: (data: RawData) => void;
  onClose: (code: number, reason: Buffer) => void;
  onError: (error: Error) => void;
};

export type OutboundMessage = string | ArrayBufferLike | Buffer | ArrayBufferView | RawData;

export function createRealtimeConnection({
  apiKey,
  model,
  onOpen,
  onMessage,
  onClose,
  onError,
}: RealtimeConnectionConfig) {
  const url = `wss://api.openai.com/v1/realtime?model=${encodeURIComponent(model)}`;
  const socket = new WebSocket(url, {
    headers: {
      Authorization: `Bearer ${apiKey}`,
      'OpenAI-Beta': 'realtime=v1',
    },
  });

  if (onOpen) {
    socket.on('open', onOpen);
  }
  socket.on('message', onMessage);
  socket.on('close', onClose);
  socket.on('error', onError);

  return {
    socket,
    send(data: OutboundMessage) {
      if (socket.readyState !== WebSocket.OPEN) {
        return false;
      }
      socket.send(data);
      return true;
    },
    close(code?: number, reason?: string) {
      const closeCode = sanitizeCloseCode(code);
      const closeReason = sanitizeCloseReason(reason);

      if (socket.readyState === WebSocket.CONNECTING) {
        socket.once('open', () => socket.close(closeCode, closeReason));
        return;
      }

      if (socket.readyState === WebSocket.CLOSING || socket.readyState === WebSocket.CLOSED) {
        return;
      }

      socket.close(closeCode, closeReason);
    },
  };
}

function sanitizeCloseCode(code?: number) {
  if (
    typeof code === 'number' &&
    Number.isInteger(code) &&
    code >= 1000 &&
    code <= 4999 &&
    code !== 1005 &&
    code !== 1006 &&
    code !== 1015
  ) {
    return code;
  }
  return 1000;
}

function sanitizeCloseReason(reason?: string) {
  if (!reason) {
    return undefined;
  }

  const encoder = new TextEncoder();
  const bytes = encoder.encode(reason);
  if (bytes.length <= 123) {
    return reason;
  }
  const truncated = bytes.slice(0, 123);
  const decoder = new TextDecoder();
  return decoder.decode(truncated);
}
