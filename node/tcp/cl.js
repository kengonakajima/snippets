// 1Mbpsぐらい送る


// 設定
var concurrency = 2;
var interval_ms = 20;
var bytes_per_write = 20 * 1024 / concurrency;
var mbps = (bytes_per_write * concurrency * (1000.0/interval_ms) * 8 ) / 1000000;
var g_host = process.argv[2]
var g_sendlen=process.argv[3];

if(!g_host) g_host="127.0.0.1";
if(!g_sendlen) g_sendlen=32; else g_sendlen=parseInt(g_sendlen);
    
var sendbuf=new Uint8Array(1024*1024);
for(var i=0;i<1024*1024;i++) sendbuf[i]=i&0xff;

console.log("start sending.. concurrency:",concurrency, "interval_ms:", interval_ms, "bytes_per_write:",bytes_per_write, "expect Mbps:", mbps );

//

function setupClient(host) {
    var cl = new net.Socket();
    cl.connect( 22222, host, function() {
        console.log("connected:",cl.remoteAddress, cl.remotePort );
        var s = "";
        var letters = "abcdefghijkljmnopqrstuvwxyz0123456789";
        for(var i=0; i<bytes_per_write; i++) s+= letters[Math.floor(Math.random()*letters.length)];
        setInterval( function() {
            var b=Buffer(g_sendlen);
            for(var i=0;i<g_sendlen;i++) {
                b[i]=sendbuf[i];
            }
            cl.write(b);
//            cl.write(s);
        }, interval_ms );
        cl.on("data", function(data) {
            recv_total_bytes += data.length;
            recv_last_bytes += data.length;
            recv_cnt++;
        });
        cl.on("close", function() {
            console.log("closed:", cl.remoteAddress, cl.remotePort );
        });
    });
}







/////
    
var recv_total_bytes=0;
var recv_last_bytes=0;
var recv_cnt=0;

var net = require("net")


for(var i=0;i<concurrency;i++) {
    setupClient(g_host);
}

setInterval( function() {
    console.log( new Date, " ", recv_total_bytes/1000000.0, "Mbytes total,", recv_last_bytes, "bytes", recv_cnt, "times" );
    recv_last_bytes=0;
}, 1000 );
