// signalingServer.js
const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });

wss.on('connection', ws => {
  console.log("on connection");
  ws.on('message', message => {
    console.log("on message:",message);
    // Broadcast any received message to all clients
    wss.clients.forEach(client => {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });
  });
  ws.on('error', e =>{
    console.log("on error:",e.code);
  });
});

console.log('Signaling server started on port 8080');
