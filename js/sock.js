// net.Socket.prototype.send = とできるかtest

var sys = require('sys');
var net = require('net');

net.Socket.prototype.send = function() {
    sys.print( "ahosdasfd\n" );
}

var server = net.createServer(function (socket) {
    socket.setEncoding("utf8");

    // 新しい接続を受けいれたとき
    socket.addListener("connect", function () {
        sys.print( "acc\n" );
    });

    // データが来たとき
    socket.addListener("data", function (data) {
        socket.send();
        sys.puts( "data:" + data + "\n" );
    });
    
    // 切れたとき
    socket.addListener("end", function () {
        sys.puts( "close\n");
    });
});

server.listen(7000, "127.0.0.1");

