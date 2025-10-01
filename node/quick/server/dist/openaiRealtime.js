"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.createRealtimeConnection = createRealtimeConnection;
const util_1 = require("util");
const ws_1 = __importDefault(require("ws"));
function createRealtimeConnection({ apiKey, model, onOpen, onMessage, onClose, onError, }) {
    const url = `wss://api.openai.com/v1/realtime?model=${encodeURIComponent(model)}`;
    const socket = new ws_1.default(url, {
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
        send(data) {
            if (socket.readyState !== ws_1.default.OPEN) {
                return false;
            }
            socket.send(data);
            return true;
        },
        close(code, reason) {
            const closeCode = sanitizeCloseCode(code);
            const closeReason = sanitizeCloseReason(reason);
            if (socket.readyState === ws_1.default.CONNECTING) {
                socket.once('open', () => socket.close(closeCode, closeReason));
                return;
            }
            if (socket.readyState === ws_1.default.CLOSING || socket.readyState === ws_1.default.CLOSED) {
                return;
            }
            socket.close(closeCode, closeReason);
        },
    };
}
function sanitizeCloseCode(code) {
    if (typeof code === 'number' &&
        Number.isInteger(code) &&
        code >= 1000 &&
        code <= 4999 &&
        code !== 1005 &&
        code !== 1006 &&
        code !== 1015) {
        return code;
    }
    return 1000;
}
function sanitizeCloseReason(reason) {
    if (!reason) {
        return undefined;
    }
    const encoder = new util_1.TextEncoder();
    const bytes = encoder.encode(reason);
    if (bytes.length <= 123) {
        return reason;
    }
    const truncated = bytes.slice(0, 123);
    const decoder = new util_1.TextDecoder();
    return decoder.decode(truncated);
}
