// 1Mbpsぐらい送る


// 設定
var concurrency = 2;
var interval_ms = 200;
var bytes_per_write = 20 * 1024 / concurrency;
var mbps = (bytes_per_write * concurrency * (1000.0/interval_ms) * 8 ) / 1000000;
var host = process.argv[2]


if(!host) {
    console.log("need host address. Ex: node cl.js 210.140.172.17");
    process.exit(1);
}
    


console.log("start sending.. concurrency:",concurrency, "interval_ms:", interval_ms, "bytes_per_write:",bytes_per_write, "expect Mbps:", mbps );

//

function setupClient() {
    var cl = new net.Socket();
    cl.connect( 22222, "210.140.72.117", function() {
        console.log("connected:",cl.remoteAddress, cl.remotePort );
        var s = "";
        var letters = "abcdefghijkljmnopqrstuvwxyz0123456789";
        for(var i=0; i<bytes_per_write; i++) s+= letters[Math.floor(Math.random()*letters.length)];
        setInterval( function() {
            cl.write(s);
        }, interval_ms );
        cl.on("data", function(data) {
            recv_total_bytes += data.length;
            recv_last_bytes += data.length;
        });
        cl.on("close", function() {
            console.log("closed:", cl.remoteAddress, cl.remotePort );
        });
    });
}







/////
    
var recv_total_bytes=0;
var recv_last_bytes=0;

var net = require("net")


for(var i=0;i<concurrency;i++) {
    setupClient();
}

setInterval( function() {
    console.log( new Date, " ", recv_total_bytes/1000000.0, "Mbytes total,", recv_last_bytes, "bytes" );
    recv_last_bytes=0;
}, 1000 );
