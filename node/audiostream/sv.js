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

var buf = new Float32Array(8192);
var idx = 0;

var blllbuf = new Float32Array(1024*1024);
var blllidx = 0;

pcm.getPcmData( "blll.wav",
                { stereo:false, sampleRate:44100 },
                function(sample,channel) {
                    blllbuf[blllidx++] = sample;
                },
                function(a,b) {
                    console.log("bllll load finished. size:", blllidx);
                }
              );
                


wss.on('connection', function (ws) {
    console.log('connected');
    // モノラル、44.1kHz
    pcm.getPcmData('nobara_hayakawa_trail_cc.mp3',
                   { stereo: false, sampleRate: 44100 },
                   function(sample, channel) {
//                       console.log("data! ", idx );

                       buf[idx++] = sample;
                       // 適当に8192サンプルずつで区切って送信する
                       if (idx == buf.length) {
                           ws.send(buf);
                           idx = 0;
                       }
                   },
                   function(a,b) {
                       console.log("endCallback a:",a);
                       console.log("endCallback b:",b);
                   }
                   
    );

    ws.on('close', function () {
        console.log('close');
    });
    ws.on("message", function(data) {
        console.log("data:",data);
    });
});