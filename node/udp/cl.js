var dgram = require('dgram');

var interval_ms = 200;
var image_size = 20 * 1024;
var divide_num = 20;
var bytes_per_message = image_size / divide_num;

var message = "";
var letters = "abcdefghijkljmnopqrstuvwxyz0123456789";
for(var i=0; i<bytes_per_message; i++) message += letters[Math.floor(Math.random()*letters.length)];


if( bytes_per_message > 1440 ) {
    console.log("warning: bytes_per_message is larger than 1440: ", bytes_per_message );
}
    

var client = dgram.createSocket('udp4');

var host = process.argv[2];
if(!host) {
    console.log("need host argument");
    process.exit(1);
}
    
var port = 22222;

var total_sent_bytes=0;
var last_sent_bytes=0;
var total_recv_bytes=0;
var last_recv_bytes=0;

setInterval( function() {
    for(var i=0;i<divide_num;i++) {
        client.send( new Buffer(message,"utf-8"), 0,0, port, host,  function(err, bytes) {
            if (err) {
                console.log("send-error, EMSGSIZE means message is larger than MTU");
                throw err;
            }
//            console.log('UDP message sent to ' + host +':'+ port, "len:", bytes);
            total_sent_bytes += bytes;
            last_sent_bytes += bytes;
        });
    };
}, interval_ms );


client.on("message", function(message,remote){
    total_recv_bytes += message.length;
    last_recv_bytes += message.length;
});

setInterval( function() {
    console.log( new Date, "Send: ", total_sent_bytes/1000000.0, "MB,(", last_sent_bytes, "B), Recv: ", total_recv_bytes/1000000.0, "MB,(", last_recv_bytes, "B)" );
    last_sent_bytes = 0;
    last_recv_bytes = 0;
}, 1000 );
