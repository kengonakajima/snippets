// echo server

var dgram = require("dgram")

var server = dgram.createSocket("udp4");


server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

server.on('message', function (message, remote) {
    console.log(remote.address + ':' + remote.port +' - ' + message);

});

server.bind(22222, '0.0.0.0');


