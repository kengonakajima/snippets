const fs = require("fs");
const sampleRate = 48000; // サンプリングレート
const soundHz = 440; // 出力したい音の周波数 (A4)
const samplePerPeriod = sampleRate/soundHz; // 1周期あたりの必要なサンプル数
const radianPerSample = Math.PI*2/samplePerPeriod; // 1サンプルあたりのsin回転角度
const volume = 3276; // 音量(最大が32767)
const bytesPerSample = 2; // 1サンプルあたりのバイト数
const duration = 1; // 継続時間
const sampleNum = duration * sampleRate; // 必要なサンプル数
const bytes=bytesPerSample*sampleNum; // 1秒分(96000バイト)
var buf = new ArrayBuffer(bytes); // 一旦メモリに出力して、それをファイルに書き出す
var dv = new DataView(buf); // リトルエンディアンでint16を書き出すためのDataView
var radian=0;
for(var i=0;i<sampleNum;i++) { // 必要なサンプル数だけループする
    const sample = Math.sin(radian)*volume; // サンプル値
    dv.setInt16(i*2, sample, true); // little endianで配列にセット
    radian+=radianPerSample;
}
fs.writeFile("raw_440hz.pcm16le", Buffer.from(buf), function(err) {if(err)console.log(err); }); // 書く
