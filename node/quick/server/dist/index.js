"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
var _a, _b, _c;
Object.defineProperty(exports, "__esModule", { value: true });
const path_1 = __importDefault(require("path"));
const dotenv_1 = __importDefault(require("dotenv"));
const express_1 = __importDefault(require("express"));
const http_1 = require("http");
const ws_1 = require("ws");
const openaiRealtime_1 = require("./openaiRealtime");
dotenv_1.default.config();
const port = Number((_a = process.env.PORT) !== null && _a !== void 0 ? _a : 3000);
const workspaceDirEnv = (_b = process.env.WORKSPACE_DIR) !== null && _b !== void 0 ? _b : './workspace';
const workspaceDir = path_1.default.resolve(process.cwd(), workspaceDirEnv);
const apiKey = process.env.OPENAI_API_KEY;
const model = (_c = process.env.OPENAI_REALTIME_MODEL) !== null && _c !== void 0 ? _c : 'gpt-4o-mini-realtime-preview-2024-12-17';
if (!apiKey) {
    console.warn('[server] OPENAI_API_KEY is not set. OpenAI integration will be disabled.');
}
const app = (0, express_1.default)();
app.get('/health', (_req, res) => {
    res.json({ status: 'ok' });
});
const server = (0, http_1.createServer)(app);
const wss = new ws_1.WebSocketServer({ server, path: '/ws' });
wss.on('connection', (socket, request) => {
    var _a;
    console.log('[ws] connection from %s', (_a = request.socket.remoteAddress) !== null && _a !== void 0 ? _a : 'unknown');
    const sendStatus = (body) => {
        const payload = { type: 'status', body };
        socket.send(JSON.stringify(payload));
    };
    if (!apiKey) {
        sendStatus('Server missing OPENAI_API_KEY. Close connection.');
        socket.close(1011, 'missing OPENAI_API_KEY');
        return;
    }
    let openaiReady = false;
    const pending = [];
    let realtime = (0, openaiRealtime_1.createRealtimeConnection)({
        apiKey,
        model,
        onOpen: () => {
            openaiReady = true;
            console.log('[openai] connection established');
            sendStatus('OpenAI connection established');
            flushPending();
        },
        onMessage: (data) => {
            var _a;
            const text = typeof data === 'string' ? data : data.toString();
            console.log('[openai] -> client', text);
            const payload = {
                type: 'openai-event',
                body: (_a = tryParseJson(text)) !== null && _a !== void 0 ? _a : text,
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
    function sendToOpenAI(message) {
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
        const parsed = tryParseJson(text);
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
function tryParseJson(value) {
    try {
        return JSON.parse(value);
    }
    catch (error) {
        return null;
    }
}
