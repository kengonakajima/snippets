var ooura = require("ooura");
var fs = require("fs");

var data = fs.readFileSync("a.LPCM16.raw"); // 「あ」の部分だけを取り出したPCM16LE音声ファイル
var len=data.length/2;  // 1サンプルあたり2バイト
var maxlen=2048; //8192;   高速フーリエ変換を使うために、入力のサイズを2の累乗に制限する
if(len>maxlen) len=maxlen; // 
var samples = new Float64Array(len); // ooura関数に入れるには float64に変換が必要
for(var i=0;i<len;i++) samples[i] = data.readInt16LE(i*2)/32767; // PCM16を浮動小数に変換
console.log("samples:",samples);
let oo = new ooura(len, {"type":"real"}); // oouraの設定. 実数を用いる
let output = oo.scalarArrayFactory(); // 出力用の空の配列を作成する
let re = oo.vectorArrayFactory(); // 解析結果の実数部が出力される配列を作成
let im = oo.vectorArrayFactory(); // 解析結果の虚数部が出力される配列を作成

oo.fft(samples.buffer, re.buffer, im.buffer);   // 高速フーリエ変換を実行。 re, imに出力される
oo.ifft(output.buffer, re.buffer, im.buffer); // 高速逆フーリエ変換を実行。 re, imからサンプルを生成する
console.log("output:",output);
console.log("re:",re);

var outpcmbuf=Buffer.alloc(len*2); // outputのfloat64配列を 16LEに変換する
for(var i=0;i<len;i++) outpcmbuf.writeInt16LE(output[i]*32767,i*2) ;
console.log("outpcmbuf",outpcmbuf);
fs.writeFileSync("a_ifft_out.LPCM16.raw",outpcmbuf); // ファイルに書き出す

// 周波数ごとの振幅を計算
var magnitude=new Float64Array(len);
for(var i=0;i<re.length;i++) {
    magnitude[i] = 20*Math.log(Math.sqrt(re[i]*re[i]+im[i]*im[i]),10);
    var s="";
    for(var j=0;j<magnitude[i]*4+20;j++) s+="*";
    console.log(i,s);
}

// 出力と入力がどれぐらい違っているか表示
//for(var i=0;i<len;i++) console.log("diff:",output[i]-samples[i]);

