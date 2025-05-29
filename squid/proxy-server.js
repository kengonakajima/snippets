const http = require('http');
const net = require('net');
const url = require('url');

const PORT = 3128;

const server = http.createServer((req, res) => {
  console.log(`HTTP ${req.method} ${req.url}`);
  
  if (req.method === 'GET' || req.method === 'POST') {
    // Regular HTTP proxy
    const options = url.parse(req.url);
    options.headers = req.headers;
    options.method = req.method;
    
    const proxyReq = http.request(options, (proxyRes) => {
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
});

// Handle CONNECT method for HTTPS and WebSocket tunneling
server.on('connect', (req, clientSocket, head) => {
  const [hostname, port] = req.url.split(':');
  console.log(`CONNECT ${hostname}:${port}`);
  
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
});

// Handle WebSocket upgrade requests
server.on('upgrade', (req, socket, head) => {
  const target = url.parse(req.url);
  const hostname = target.hostname || req.headers.host?.split(':')[0];
  const port = target.port || req.headers.host?.split(':')[1] || 80;
  console.log(`WebSocket upgrade to ${hostname}:${port}`);
  
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
});

server.listen(PORT, () => {
  console.log(`HTTP/WebSocket Proxy server running on port ${PORT}`);
  console.log('Usage:');
  console.log('  HTTP: curl -x http://localhost:3128 http://example.com');
  console.log('  WebSocket: wscat --proxy http://localhost:3128 -c ws://localhost:8080');
});

server.on('error', (err) => {
  console.error('Server error:', err);
});