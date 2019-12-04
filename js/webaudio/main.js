/*
 * main.js
 */

window.AudioContext = window.AudioContext || window.webkitAudioContext;  
var context = new AudioContext();

// Audio 用の buffer を読み込む
var getAudioBuffer = function(url, fn) {  
  var req = new XMLHttpRequest();
  // array buffer を指定
  req.responseType = 'arraybuffer';

  req.onreadystatechange = function() {
    if (req.readyState === 4) {
      if (req.status === 0 || req.status === 200) {
        // array buffer を audio buffer に変換
        context.decodeAudioData(req.response, function(buffer) {
          // コールバックを実行
          fn(buffer);
        });
      }
    }
  };

  req.open('GET', url, true);
  req.send('');
};

var g_last_played_source = null;

// サウンドを再生
var playSound = function(buffer) {
    console.log(buffer);

    var source = context.createBufferSource();
    // buffer をセット
    source.buffer = buffer;
    // context に connect
    source.connect(context.destination);
    // 再生
    source.start(0);

    g_last_played_source = source;
};

var t=0;

function genSamples(outbuf,frameCount,hz) {
    var t_incr = 2 * Math.PI * hz / 48000.0;
    var b0 = outbuf.getChannelData(0);
    var b1 = outbuf.getChannelData(1);
    for (var i = 0; i < frameCount; i++) {    
        b0[i] = Math.sin(t);
        b1[i] = b0[i]
        t+=t_incr
    }
}

function makeMemoryAudioBuf(frameCount,hz) {
    var ch=2;
    var outbuf = context.createBuffer( ch, frameCount, context.sampleRate );
    genSamples(outbuf,frameCount,hz);
    return outbuf;
}
var membuf = makeMemoryAudioBuf(context.sampleRate*2,440);

function shiftSamples(n) {
    for(var i=n;i<samples_r.length;i++) {
        samples_r[i-n]=samples_r[i];
        samples_l[i-n]=samples_l[i];
    }
    samples_used-=n;
}

var sn = context.createScriptProcessor(256,2,2);
console.log("sn:",sn);
sn.onaudioprocess = function(audioProcessingEvent) {
    var inputBuffer = audioProcessingEvent.inputBuffer;
    var outputBuffer = audioProcessingEvent.outputBuffer;
    var out0 = outputBuffer.getChannelData(0);
    var out1 = outputBuffer.getChannelData(1);
    if(samples_used>=inputBuffer.length) {
        console.log("enough samples:",samples_used, samples_r.length, samples_l.length);
        for (var i = 0; i < inputBuffer.length; i++) {
            out0[i] = samples_r[i];
            out1[i] = samples_l[i];
        }
        shiftSamples(inputBuffer.length);
    } else {
        console.log("need samples:",samples_used);        
        for (var i = 0; i < inputBuffer.length; i++) {
            out0[i] = Math.random()*0.1;
            out1[i] = Math.random()*0.1;
        }

    }
}
var dummysrc = context.createConstantSource();
dummysrc.connect(sn);
sn.connect(context.destination);

var samples_r = new Float32Array(48000);
var samples_l = new Float32Array(48000);

var gentime=0;
var intvl=50;
var samples_used = 0;
var hz=440;
function genSamples() {
    hz+=1;
    var t_incr = 2 * Math.PI * hz/2 / 48000.0;    
    var n = 48000 * intvl / 1000;
    if(samples_used>40000) {
        console.log("too much samples:",samples_used);
        shiftSamples(n);
    }
    for(var i=0;i<n;i++) {
        samples_r[samples_used+i] = Math.sin(gentime)*0.1;
        samples_l[samples_used+i] = Math.sin(gentime)*0.1;
        gentime += t_incr;
    }
    samples_used+=n;
    console.log("used:",samples_used);
}

setInterval( genSamples, intvl );


// main
window.onload = function() {  
    // サウンドを読み込む
    getAudioBuffer('bl.wav', function(buffer) { // se.mp3もいける
        // 読み込み完了後にボタンにクリックイベントを登録
        var btn = document.getElementById('btn');
        btn.onclick = function() {
            // サウンドを再生
            playSound(buffer);
        };
        var btn2 = document.getElementById("btn2");
        btn2.onclick = function() {
            playSound(membuf);
        }
        var btn3 = document.getElementById("btn3");
        btn3.onclick = function() {
            console.log("PPPPP", g_last_played_source );
            if(g_last_played_source) {
                g_last_played_source.stop(0);
            }
            dummysrc.stop();
            
        }
        var btnstream = document.getElementById("stream");
        btnstream.onclick = function() {
            dummysrc.start(0)   ;
        }
    });

    setInterval(function() {
        document.getElementById("stat").innerHTML="used:"+samples_used;        
    },100)

    
};