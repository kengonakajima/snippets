const Readable=require("stream").Readable; 
const Speaker=require("speaker");

const sine=new Readable(); // 
sine.t=0;    // 音波を生成する際の時刻カウンター
sine._read = function(n) { // Speakerモジュールで新しいサンプルデータが必要になったら呼び出されるコールバック関数 n:バイト数
    var sampleNum = n/2; // サンプルデータの数を計算する。16ビットPCMなのでnを2バイトで割る
    var u8ary = new Uint8Array(n); // 出力用データの配列
    var dv=new DataView(u8ary.buffer); // 16ビットリトルエンディアン整数の出力用
    for(var i=0;i<sampleNum;i++) { // 必要なサンプリングデータの数だけループさせる
        this.t += 0.1; // 1サンプルごとに時間を進める(2PI=3.14*2=6.28進めると1周期)
        dv.setInt16(i*2,Math.sin(this.t)*20000,true); // 振幅を-20000から20000の間で出力
    }
    this.push(u8ary); // 最終的な値を出力
}

const spk=new Speaker({ 
    channels: 1, // チャンネル数は1(モノラル)
    bitDepth: 16, // サンプリングデータのビット数は16 (デフォルトはリトルエンディアン)
    sampleRate: 48000, // サンプリングレート(Hz)
});

sine.pipe(spk); 

