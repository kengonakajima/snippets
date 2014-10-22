var pcm = require('pcm');

var wss = new (require('ws').Server)({
    server: require('http').createServer(function(req, res) {
        console.log("http req.url:",req.url);
        
        if( req.url == "/jquery.js" ) {
            res.writeHead(200, { 'Content-Type': 'text/javascript'});
            res.end(require('fs').readFileSync('jquery.js'));            
        } else {
            res.writeHead(200, { 'Content-Type': 'text/html'});
            res.end(require('fs').readFileSync('index.html'));
        }
    }).listen(8888)
});

var bgmbuf = new Float32Array(1024*1024);
var bgmidx = 0;

var blllbuf = new Float32Array(16384);
var blllidx = 0;

var mixlen = 16384;
var mixbuf = new Float32Array(mixlen);

// 送信用のバッファとmix用のバッファは別なので、常にバッファの先頭から書き込むのでOK
// mixlenより長い音は再生できない
function play(buf) {
    for(var i=0;i<buf.length && i < mixlen ;i++) {
        mixbuf[i] += buf[i];
    }
}
// 長さの分だけシフトしてバッファを返す
function shiftMix(len) {
    var out = new Float32Array(len)
    var i;
    for(i=0;i<len && i < mixlen;i++) {
        out[i] = mixbuf[i];
    }
    for(i=len;i<mixlen;i++) {
        mixbuf[i-len] = mixbuf[i];
    }
    for(i=mixlen-len;i<mixlen;i++) {
        mixbuf[i] = 0;
    }
    return out;
}

var interval_ms = 16.666;
var hz = 44100;
var sample_per_sec = hz * 1;
var sample_per_ms = sample_per_sec / 1000.0;
var sample_per_cb = parseInt(sample_per_ms * interval_ms);
console.log( "Sample per callback:", sample_per_cb );

var connections = new Array();

function mixSenderCallback() {
    var to_send = shiftMix(sample_per_cb);
    if( connections.length > 0 ) console.log( "sending samples:", sample_per_cb, "conns:", connections.length, to_send[0] );    
    var i;
    for(i=0;i<connections.length;i++) {
        var c = connections[i];
        c.send(to_send);
    }
}
setInterval( mixSenderCallback, interval_ms );

pcm.getPcmData( "blll.wav",
                { stereo:false, sampleRate:44100 },
                function(sample,channel) {
                    blllbuf[blllidx++] = sample;
                },
                function(a,b) {
                    console.log("bllll load finished. size:", blllbuf.length );
                }
              );
                

// モノラル、44.1kHz
pcm.getPcmData('gym1.mp3',
               { stereo: false, sampleRate: 44100 },
               function(sample, channel) {
                   bgmbuf[bgmidx++] = sample;
               },
               function(a,b) {
                   console.log("bgm load finished. size:", bgmbuf.length );
               }
              );

var bgm_idx=0;
function bgmStreamCallback() {
    var to_play = new Float32Array(sample_per_cb);
    for(var i=0;i<sample_per_cb;i++) {
        to_play[i] = bgmbuf[bgm_idx++];
        if(bgm_idx>bgmbuf.length) {
            bgm_idx=0;
            console.log( "BGM looped!");
        }
    }
    play(to_play);
}

function regConnection(c) {
    connections.push(c);
}
function delConnection(c) {
    var at = connections.indexOf(c);
    connections.splice(at,1);
}

wss.on('connection', function (ws) {
    console.log('connected');
    regConnection(ws);
    setInterval( bgmStreamCallback, interval_ms );
    
    ws.on('close', function () {
        console.log('close');
        delConnection(ws);
    });
    ws.on("message", function(data) {
        console.log("data:",data);
        
        if( data == 65 ) { // a
            play(blllbuf);            
        }
        if( data == 66 ) { // b
            var i;
            for(i=0;i<1000;i++) console.log( mixbuf[i] );
        }
    });
});