const fs = require('fs');
const https = require('https');
const WebSocket = require('ws');

const server = https.createServer({
  cert: fs.readFileSync('/etc/ssl/certs/サーバ証明書'),
  ca: fs.readFileSync('/etc/ssl/certs/中間証明書'),
  key: fs.readFileSync('/etc/ssl/certs/キーファイル')
});

const wss = new WebSocket.Server({ server });

wss.on('connection', function connection(ws,req) {
    //connect時
    const ip = req.connection.remoteAddress;
    console.log('---connect from ' + ip);
    ws.send('Hello from wssServer!');

    //receive message
    ws.on('message', function incoming(message) {
        console.log(message);
    });

    //標準入力で入力したメッセージを接続中のclientに送る
    process.stdin.on('data', function (chunk) {
      var sendmessage = chunk.toString();
      ws.send(sendmessage);
    });
});

//8080ポートでサーバ待ち受け
server.listen(8080);