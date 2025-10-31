
var net = require('net');

var server = net.createServer(function(socket) {
    console.log("connect");
    socket.on("error", function(e) {
        console.log("error:",e);
    });
    socket.on("close",function() {
        console.log("close");
    });
    socket.on("data",function(data) {
    })
})

server.listen(22222);
