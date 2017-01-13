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

// サウンドを再生
var playSound = function(buffer) {
    console.log(buffer);
  // source を作成
  var source = context.createBufferSource();
  // buffer をセット
  source.buffer = buffer;
  // context に connect
  source.connect(context.destination);
  // 再生
  source.start(0);
};

function makeMemoryAudioBuf() {
    var ch=2;
    var frameCount = context.sampleRate*2.0;
    var outbuf = context.createBuffer( ch, frameCount, context.sampleRate );

    for (var chi = 0; chi < ch; chi++) {
        var b = outbuf.getChannelData(chi);
        for (var i = 0; i < frameCount; i++) {
            b[i] = Math.sin(i/10.0);
        }
    }    
    return outbuf;
}
var membuf = makeMemoryAudioBuf();

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
    });
};