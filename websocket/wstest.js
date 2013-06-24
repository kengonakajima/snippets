var connection = new WebSocket('ws://106.187.55.104:8999/echo', ['binary'] );
connection.binaryType = "arraybuffer";
console.log("ws", connection);

connection.onopen = function () {
    connection.send('Ping'); // Send the message 'Ping' to the server
};

connection.onerror = function (error) {
    console.log('WebSocket Error ' + error);
};

connection.onmessage = function (e) {
    console.log('Server: ' + e.data.byteLength);
    var dv = new DataView(e.data);
    for(var i=0;i<e.data.byteLength;i++) {
        console.log("byte:",i,dv.getInt8(i) );
        $("#log").append( "byte:" + i + "," + dv.getInt8(i) + " <br>" );
    }
};


