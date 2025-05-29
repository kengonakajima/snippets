const http = require('http');
const https = require('https');
const net = require('net');
const url = require('url');
const fs = require('fs');

const HTTP_PORT = 3128;
const HTTPS_PORT = 3129;

// SSL証明書の設定
const SSL_OPTIONS = {
  key: fs.readFileSync('/etc/letsencrypt/live/mwf-test2.mbe.dev.monobit.net/privkey.pem'),
  cert: fs.readFileSync('/etc/letsencrypt/live/mwf-test2.mbe.dev.monobit.net/fullchain.pem')
};

// プロキシハンドラー関数
function handleProxyRequest(req, res) {
  console.log(`${req.socket.encrypted ? 'HTTPS' : 'HTTP'} ${req.method} ${req.url}`);
  
  if (req.method === 'GET' || req.method === 'POST' || req.method === 'PUT' || req.method === 'DELETE' || req.method === 'PATCH') {
    // Regular HTTP proxy
    const options = url.parse(req.url);
    options.headers = req.headers;
    options.method = req.method;
    
    // HTTPSターゲットの場合はhttpsモジュールを使用
    const requestModule = options.protocol === 'https:' ? https : http;
    
    const proxyReq = requestModule.request(options, (proxyRes) => {
      res.writeHead(proxyRes.statusCode, proxyRes.headers);
      proxyRes.pipe(res);
    });
    
    proxyReq.on('error', (err) => {
      console.error('Proxy request error:', err);
      res.writeHead(500);
      res.end('Proxy Error');
    });
    
    req.pipe(proxyReq);
  } else {
    res.writeHead(405);
    res.end('Method not allowed');
  }
}

// CONNECTハンドラー関数
function handleConnect(req, clientSocket, head) {
  const [hostname, port] = req.url.split(':');
  console.log(`${clientSocket.encrypted ? 'HTTPS' : 'HTTP'} CONNECT ${hostname}:${port}`);
  
  const serverSocket = net.createConnection(port || 80, hostname, () => {
    clientSocket.write('HTTP/1.1 200 Connection Established\r\n' +
                       'Proxy-agent: Node.js-Proxy\r\n' +
                       '\r\n');
    serverSocket.write(head);
    serverSocket.pipe(clientSocket);
    clientSocket.pipe(serverSocket);
  });
  
  serverSocket.on('error', (err) => {
    console.error('Server socket error:', err);
    clientSocket.end();
  });
  
  clientSocket.on('error', (err) => {
    console.error('Client socket error:', err);
    serverSocket.end();
  });
}

// WebSocketアップグレードハンドラー関数
function handleUpgrade(req, socket, head) {
  const target = url.parse(req.url);
  const hostname = target.hostname || req.headers.host?.split(':')[0];
  const port = target.port || req.headers.host?.split(':')[1] || 80;
  console.log(`${socket.encrypted ? 'HTTPS' : 'HTTP'} WebSocket upgrade to ${hostname}:${port}`);
  
  const serverSocket = net.createConnection(port, hostname, () => {
    // Forward the upgrade request
    const requestLine = `${req.method} ${target.path || '/'} HTTP/${req.httpVersion}\r\n`;
    const headers = Object.keys(req.headers)
      .map(key => `${key}: ${req.headers[key]}`)
      .join('\r\n');
    
    serverSocket.write(requestLine + headers + '\r\n\r\n');
    if (head.length > 0) {
      serverSocket.write(head);
    }
    
    // Pipe data both ways
    serverSocket.pipe(socket);
    socket.pipe(serverSocket);
  });
  
  serverSocket.on('error', (err) => {
    console.error('WebSocket server error:', err);
    socket.end();
  });
  
  socket.on('error', (err) => {
    console.error('WebSocket client error:', err);
    serverSocket.end();
  });
}

// HTTPサーバー作成
const httpServer = http.createServer(handleProxyRequest);
httpServer.on('connect', handleConnect);
httpServer.on('upgrade', handleUpgrade);

// HTTPSサーバー作成
const httpsServer = https.createServer(SSL_OPTIONS, handleProxyRequest);
httpsServer.on('connect', handleConnect);
httpsServer.on('upgrade', handleUpgrade);

// サーバー起動
httpServer.listen(HTTP_PORT, () => {
  console.log(`HTTP Proxy server running on port ${HTTP_PORT}`);
  console.log('Usage:');
  console.log(`  HTTP: curl -x http://localhost:${HTTP_PORT} http://example.com`);
});

httpsServer.listen(HTTPS_PORT, () => {
  console.log(`HTTPS Proxy server running on port ${HTTPS_PORT}`);
  console.log('Usage:');
  console.log(`  HTTPS: curl -x https://localhost:${HTTPS_PORT} https://example.com`);
  console.log(`  CONNECT: curl -x https://mwf-test2.mbe.dev.monobit.net:${HTTPS_PORT} https://example.com`);
});

// エラーハンドリング
httpServer.on('error', (err) => {
  console.error('HTTP Server error:', err);
});

httpsServer.on('error', (err) => {
  console.error('HTTPS Server error:', err);
});