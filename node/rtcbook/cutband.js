var ooura = require("ooura");
var fs = require("fs");
var fixrange16=function(v) { if(v<-32768) return -32768; else if(v>32767) return 32767; return v; }

// step 1. 「あ」の声を2048サンプル読み込む (ooura.jsと同じ)
var data = fs.readFileSync("a.LPCM16.raw");
var len=2048;
var samples = new Float64Array(len);
for(var i=0;i<len;i++) samples[i] = data.readInt16LE(i*2)/32767;

// step 2. FFTで周波数領域に変換する
let oo = new ooura(len, {"type":"real"});
let output = oo.scalarArrayFactory();
let re = oo.vectorArrayFactory();
let im = oo.vectorArrayFactory();
oo.fft(samples.buffer, re.buffer, im.buffer);

// step 3. 700Hz~2000Hzにあたるビンのre,imを0にする。1ビンは 48000/2048 = 23.4375Hz
var binHz = 48000/2048;
var lo=Math.round(700/binHz), hi=Math.round(2000/binHz);
console.log("cut bins:", lo, "-", hi);
for(var i=lo;i<=hi;i++) { re[i]=0; im[i]=0; }

// step 4. IDFTで時間領域に戻してファイルに書き出す
oo.ifft(output.buffer, re.buffer, im.buffer);
var outpcmbuf=Buffer.alloc(len*2);
for(var i=0;i<len;i++) outpcmbuf.writeInt16LE(fixrange16(output[i]*32767),i*2);
fs.writeFileSync("a_cutband_out.LPCM16.raw",outpcmbuf);

// step 5. 加工後の音をもう一度FFTして、*のグラフで確認する
var re2 = oo.vectorArrayFactory();
var im2 = oo.vectorArrayFactory();
oo.fft(output.buffer, re2.buffer, im2.buffer);
for(var i=0;i<re2.length/4;i++) {
    var mag = 20*Math.log10(Math.sqrt(re2[i]*re2[i]+im2[i]*im2[i]));
    var s="";
    for(var j=0;j<mag*4+20;j++) s+="*";
    console.log(i,s);
}
