import { useEffect, useMemo, useRef, useState } from 'react';
import './App.css';

type LogEntry = {
  direction: 'sent' | 'received' | 'status';
  payload: string;
};

function App() {
  const [connectionState, setConnectionState] = useState<'connecting' | 'open' | 'closed'>('connecting');
  const [messages, setMessages] = useState<LogEntry[]>([]);
  const [outgoing, setOutgoing] = useState('');
  const socketRef = useRef<WebSocket | null>(null);

  const wsUrl = useMemo(() => {
    const explicit = import.meta.env.VITE_SERVER_WS_URL as string | undefined;
    if (explicit) {
      return explicit;
    }
    const { protocol, hostname } = window.location;
    const wsProtocol = protocol === 'https:' ? 'wss:' : 'ws:';
    const port = import.meta.env.VITE_SERVER_PORT ?? '3000';
    return `${wsProtocol}//${hostname}:${port}/ws`;
  }, []);

  useEffect(() => {
    const socket = new WebSocket(wsUrl);
    socketRef.current = socket;
    setConnectionState('connecting');
    setMessages((prev) => [...prev, { direction: 'status', payload: `connecting to ${wsUrl}` }]);

    socket.addEventListener('open', () => {
      setConnectionState('open');
      setMessages((prev) => [...prev, { direction: 'status', payload: 'connection opened' }]);
    });

    socket.addEventListener('message', (event) => {
      const payload = typeof event.data === 'string' ? event.data : '[binary data]';
      setMessages((prev) => [...prev, { direction: 'received', payload }]);
    });

    socket.addEventListener('close', (event) => {
      setConnectionState('closed');
      setMessages((prev) => [
        ...prev,
        { direction: 'status', payload: `connection closed (code=${event.code})` },
      ]);
    });

    socket.addEventListener('error', (event) => {
      console.error('[ws] error event', event);
      setMessages((prev) => [...prev, { direction: 'status', payload: 'connection error' }]);
    });

    return () => {
      socket.close();
    };
  }, [wsUrl]);

  const handleSend = () => {
    if (!socketRef.current || socketRef.current.readyState !== WebSocket.OPEN) {
      setMessages((prev) => [...prev, { direction: 'status', payload: 'socket not ready' }]);
      return;
    }
    if (!outgoing.trim()) {
      return;
    }
    const payload = JSON.stringify({ type: 'user-message', body: outgoing });
    socketRef.current.send(payload);
    setMessages((prev) => [...prev, { direction: 'sent', payload }]);
    setOutgoing('');
  };

  return (
    <div className="app">
      <header>
        <h1>Realtime Agent Console</h1>
        <span className={`status status-${connectionState}`}>{connectionState}</span>
      </header>
      <section className="controls">
        <textarea
          value={outgoing}
          onChange={(event) => setOutgoing(event.target.value)}
          placeholder="Type a message to send to the server"
          rows={4}
        />
        <button onClick={handleSend} disabled={connectionState !== 'open'}>
          Send
        </button>
      </section>
      <section className="log">
        <h2>Log</h2>
        <ul>
          {messages.map((entry, index) => (
            <li key={index} className={`log-${entry.direction}`}>
              <span className="log-direction">{entry.direction}</span>
              <span className="log-payload">{entry.payload}</span>
            </li>
          ))}
        </ul>
      </section>
    </div>
  );
}

export default App;
