const recorder = require('node-record-lpcm16'); // nodeモジュールを読み込む
const fs=require("fs");

const RINGBUF_LEN=48000*2;
const g_samples_ring=new Int16Array(RINGBUF_LEN); // lpcm16

let g_rec_cnt=0;
recorder
  .record({
    sampleRate: 48000, // マイクデバイスのサンプリングレートを指定
    channels: 1,  // チャンネル数を指定(モノラル)              
    recordProgram: 'rec', // 録音用のバックエンドプログラム名を指定
  })
  .stream()
  .on('error', console.error) // エラーが起きたときにログを出力する
  .on('data', function(data) { // マイクからデータを受信する無名コールバック関数
    const vol = Math.abs(data.readInt16LE());  // 配列の先頭のサンプリングデータをひとつ読み込み、音量を得る
    const ntimes = vol / 512; // 音量が0~32767の値で得られるので512で割る(0~63)
    const bar = "*".repeat(ntimes); // アスタリスク文字を、音量に応じて0~63回繰り返す
    const sampleNum=data.length/2;
    for(let i=0;i<sampleNum;i++) {
      const sample=data.readInt16LE(i*2);
      g_samples_ring[g_rec_cnt%RINGBUF_LEN]=sample;
      g_rec_cnt++;      
    }
    console.log("rec: volume:", bar, "total:",g_rec_cnt,"len:",data.length);//,g_samples.slice(0,8).join(","));
  });
console.log('Listening, press Ctrl+C to stop.');


const Readable=require("stream").Readable; 
const Speaker=require("speaker");

let g_play_cnt=0;

const sine=new Readable(); // 
sine.t=0;    // 音波を生成する際の時刻カウンター
sine._read = function(n) { // Speakerモジュールで新しいサンプルデータが必要になったら呼び出されるコールバック関数 n:バイト数
  console.log("play:",g_play_cnt,"diff:",(g_rec_cnt-g_play_cnt));
  var sampleNum = n/2; // サンプルデータの数を計算する。16ビットPCMなのでnを2バイトで割る
  var u8ary = new Uint8Array(n); // 出力用データの配列
  var dv=new DataView(u8ary.buffer); // 16ビットリトルエンディアン整数の出力用
  if(g_rec_cnt>=sampleNum*2) {
    for(var i=0;i<sampleNum;i++) { // 必要なサンプリングデータの数だけループさせる
      const sample=g_samples_ring[g_play_cnt%RINGBUF_LEN];
      g_play_cnt++;      
      dv.setInt16(i*2,sample,true);
//        this.t += 0.1; // 1サンプルごとに時間を進める(2PI=3.14*2=6.28進めると1周期)
//        dv.setInt16(i*2,Math.sin(this.t)*20000,true); // 振幅を-20000から20000の間で出力      
    }
  } else {
    console.log("need more samples in ring buffer, play sine wave");
    // 音が足りないのでピー音
    for(var i=0;i<sampleNum;i++) { // 必要なサンプリングデータの数だけループさせる
      this.t += 0.1; // 1サンプルごとに時間を進める(2PI=3.14*2=6.28進めると1周期)
      dv.setInt16(i*2,Math.sin(this.t)*20000,true); // 振幅を-20000から20000の間で出力
    }
  }
  this.push(u8ary); // 最終的な値を出力
}

const spk=new Speaker({ 
    channels: 1, // チャンネル数は1(モノラル)
    bitDepth: 16, // サンプリングデータのビット数は16 (デフォルトはリトルエンディアン)
    sampleRate: 48000, // サンプリングレート(Hz)
});

sine.pipe(spk); 

