
var net = require('net');

var g_sockets=[];

var server = net.createServer(function(socket) {
    g_sockets.push(socket);
    console.log("connect. socknum:", g_sockets.length);
    
    socket.on("error", function(e) {
        console.log("error:",e);
    });
    socket.on("close",function() {
        for(var i=0;i<g_sockets.length;i++) {
            if(g_sockets[i]==socket) {
                g_sockets.splice(i,1);
                break;
            }
        }
        console.log("closed. socknum:",g_sockets.length);
    });
    socket.on("data",function(data) {
        socket.write(data);
        for(var i=0;i<g_sockets.length;i++) {
            g_sockets[i].write(data);
//            console.log("data len:",i, data.length, new Date().getTime(), g_sockets.length );            
        }
    })
})

server.listen(22222);
