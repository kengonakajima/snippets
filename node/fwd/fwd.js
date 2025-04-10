const net = require('net');

const dest_host = process.argv[2];
const dest_ports = process.argv.slice(3);
console.log("dest_host:",dest_host,"dest_ports:",dest_ports);

if(dest_ports==null || dest_ports.length==0) {
  console.log("invalid arg, need host port port ..");
  process.exit(1);
}
function setup(host,port) {
  const server = net.createServer((localSocket) => {
    // フォワード先への接続を作成
    const remoteSocket = new net.Socket();

    remoteSocket.connect(port,host, () => {
      console.log("connect:",port,host);
      localSocket.on('data', (data) => {
        remoteSocket.write(data);
      });

      remoteSocket.on('data', (data) => {
        localSocket.write(data);
      });
    });

    localSocket.on('error', (err) => { 
      console.log("local error:",err);
      console.error(`Local socket error: ${err}`);
      remoteSocket.end();
    });

    remoteSocket.on('error', (err) => {
      console.log("remote error:",port,host,err);           
      console.error(`Remote socket error: ${err}`);
      localSocket.end();
    });

    localSocket.on('close', () => {
      remoteSocket.end();
    });

    remoteSocket.on('close', () => {
      console.log("close:",port,host);
      localSocket.end();
    });
  });
  server.listen(port,"127.0.0.1", () => {
    console.log(`Server listening on 127.0.0.1:${port}`);
  });
}

for(const dest_port of dest_ports) {
  setup(dest_host,dest_port);
}

