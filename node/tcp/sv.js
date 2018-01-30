
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
        socket.write(data);
        if(data.length<50) console.log("data:",data);
    })
})

server.listen(22222);
