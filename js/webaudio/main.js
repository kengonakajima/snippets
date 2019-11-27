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

//var mediaSource = new MediaSource();
var g_tone=1;

var sn = context.createScriptProcessor(256,1,1);
console.log("sn:",sn);
sn.onaudioprocess = function(audioProcessingEvent) {
    var t_incr = 2 * Math.PI * 440.0*3 / 48000.0;
    var inputBuffer = audioProcessingEvent.inputBuffer;
    var outputBuffer = audioProcessingEvent.outputBuffer;
    for (var channel = 0; channel < outputBuffer.numberOfChannels; channel++) {
        var inputData = inputBuffer.getChannelData(channel);
        var outputData = outputBuffer.getChannelData(channel);
        for (var sample = 0; sample < inputBuffer.length; sample++) {
            outputData[sample] = inputData[sample];
            outputData[sample] += (Math.sin(t)*0.2);
            t+=t_incr/2.0;
        }
    }
}
var dummysrc = context.createConstantSource();
dummysrc.connect(sn);
sn.connect(context.destination);




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
        }
        var btnstream = document.getElementById("stream");
        btnstream.onclick = function() {
            dummysrc.start(0)   ;
        }
    });

    
};