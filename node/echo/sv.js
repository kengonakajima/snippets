
var net = require('net');

var server = net.createServer(function(socket) {
    socket.on("error", function(e) {
        console.log("error:",e);
    });
    socket.on("close",function() {
        console.log("close");
    });
    
    socket.pipe(socket);
})

server.listen(22222);
