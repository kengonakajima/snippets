// 1Mbpsぐらい送る


// 設定
var interval_ms = 200;
var bytes_per_write = 20 * 1024;
var mbps = (bytes_per_write * (1000.0/interval_ms) * 8 ) / 1000000;
var host = process.argv[2]

if(!host) {
    console.log("need host address. Ex: node cl.js 210.140.172.17");
    process.exit(1);
}
    


console.log("start sending.. expect Mbps:", mbps );

//

var recv_total_bytes=0;
var recv_last_bytes=0;

var net = require("net")
var cl = new net.Socket();
cl.connect( 22222, "210.140.72.117", function() {
    console.log("connected");

    var s = "";
    var letters = "abcdefghijkljmnopqrstuvwxyz0123456789";
    for(var i=0; i<bytes_per_write; i++) s+= letters[Math.floor(Math.random()*letters.length)];

    setInterval( function() {
        cl.write(s);
    }, interval_ms );

    setInterval( function() {
        console.log( new Date, " ", recv_total_bytes/1000000.0, "Mbytes total,", recv_last_bytes, "bytes" );
        recv_last_bytes=0;
    }, 1000 );
});



cl.on("data", function(data) {
    recv_total_bytes += data.length;
    recv_last_bytes += data.length;
});

cl.on("close", function() {
    console.log("closed");
});

