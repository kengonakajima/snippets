// echo server

var host = process.argv[2]

if(!host) {
    console.log("need host argument");
    process.exit(1);
}

var dgram = require("dgram")

var server = dgram.createSocket("udp4");


server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

var total_recv_bytes=0;
var last_recv_bytes=0;

server.on('message', function (message, remote) {
//    console.log(remote.address + ':' + remote.port +' - ' + message);
    server.send(message,0,message.length, remote.port, remote.host, function(err,bytes) {
//        console.log("sent ", bytes, " bytes to client:", remote.host, remote.port );
        total_recv_bytes += message.length;
        last_recv_bytes += message.length;
    });

});

setInterval( function() {
    console.log("Recv:", total_recv_bytes/1000000.0, "MB(",last_recv_bytes, "B)");
    last_recv_bytes =0;
},1000 );

server.bind(22222, host);


