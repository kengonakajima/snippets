const dgram = require('dgram');
const server = dgram.createSocket('udp4');

server.on('error', (err) => {
  console.log(`サーバーエラー：\n${err.stack}`);
  server.close();
});

server.on('message', (msg, rinfo) => {
  console.log(`サーバーがメッセージを受信：${msg} from ${rinfo.address}:${rinfo.port}`);
  // クライアントにメッセージをエコーバック
  server.send(msg, rinfo.port, rinfo.address, (err) => {
    if (err) {
      server.close();
    }
  });
});

server.on('listening', () => {
  const address = server.address();
  console.log(`サーバーが ${address.address}:${address.port} でリスニング`);
});

// 41234ポートでサーバーをバインド（起動）
server.bind(41234);

