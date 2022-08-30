const recorder = require('node-record-lpcm16'); // nodeモジュールを読み込む
const fs=require("fs");

const g_samples=[]; // 録音したサンプルをlpcm16形式で保存する配列

let g_rec_max_sample=0, g_play_max_sample=0;

// "******      " のような文字列を返す
function getVolumeBar(l16sample) {
  const vol=Math.abs(l16sample);
  const bar = vol / 1024;
  const space = 32-bar;
  return "*".repeat(bar)+" ".repeat(space); 
}
recorder
  .record({
    sampleRate: 48000, // マイクデバイスのサンプリングレートを指定
    channels: 1,  // チャンネル数を指定(モノラル)              
    recordProgram: 'rec', // 録音用のバックエンドプログラム名を指定
  })
  .stream()
  .on('error', console.error) // エラーが起きたときにログを出力する
  .on('data', function(data) { // マイクからデータを受信する無名コールバック関数
    const sampleNum=data.length/2; // dataに含まれるサンプル数
    // 得られたデータをバッファにpushする
    g_rec_max_sample=0;
    for(let i=0;i<sampleNum;i++) {
      const sample=data.readInt16LE(i*2);
      g_samples.push(sample);
      if(sample>g_rec_max_sample) g_rec_max_sample=sample; // ついでに、最大音量を記録
    }
  });

const Readable=require("stream").Readable; 
const Speaker=require("speaker");

const sine=new Readable(); // 
sine._read = function(n) { // Speakerモジュールで新しいサンプルデータが必要になったら呼び出されるコールバック関数 n:バイト数
  var sampleNum = n/2; // サンプルデータの数を計算する。16ビットPCMなのでnを2バイトで割る
  var u8ary = new Uint8Array(n); // 出力用データの配列
  var dv=new DataView(u8ary.buffer); // 16ビットリトルエンディアン整数の出力用
  if(g_samples.length>9600) { // 固定長のジッタバッファ (200ms)
    // バッファにあるデータを再生用配列に転送する
    g_play_max_sample=0;
    for(var i=0;i<sampleNum;i++) { // 必要なサンプリングデータの数だけループさせる
      const sample=g_samples.shift();
      dv.setInt16(i*2,sample,true);
      if(sample>g_play_max_sample) g_play_max_sample=sample; // ついでに、最大音量を記録
    }
  } else {
    console.log("need more samples!");
    // 音が足りないので無音を再生
    for(var i=0;i<sampleNum;i++) dv.setInt16(i*2,0,true); // 必要なサンプリングデータの数だけループさせる
  }
  this.push(u8ary); // 最終的な値を出力
}

const spk=new Speaker({ 
    channels: 1, // チャンネル数は1(モノラル)
    bitDepth: 16, // サンプリングデータのビット数は16 (デフォルトはリトルエンディアン)
    sampleRate: 48000, // サンプリングレート(Hz)
});

sine.pipe(spk); 

setInterval(function() {
  console.log("rec:",getVolumeBar(g_rec_max_sample),
              "play:",getVolumeBar(g_play_max_sample),
              "buffer:",g_samples.length);
},50);
