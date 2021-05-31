var ooura = require("ooura");
var fs = require("fs");

var data = fs.readFileSync("a.LPCM16.raw");
console.log("data:",data,data.readInt16LE(0));
var maxlen=2048;//8192;
var len=data.length/2;  
if(len>maxlen) len=maxlen;
var samples = new Float64Array(len);
for(var i=0;i<len;i++) samples[i] = data.readInt16LE(i*2)/32767;
console.log("samples:",samples);
let oo = new ooura(len, {"type":"real", "radix":4});
let output = oo.scalarArrayFactory();
let re = oo.vectorArrayFactory();
let im = oo.vectorArrayFactory();

oo.fft(samples.buffer, re.buffer, im.buffer);   //populates re and im from input
console.log("real:",re,"img:",im);
oo.ifft(output.buffer, re.buffer, im.buffer); //populates output from re and im

console.log("output:",output);

var outpcmbuf=Buffer.alloc(len*2);
for(var i=0;i<len;i++) outpcmbuf.writeInt16LE(output[i]*32767,i*2) ;
console.log("outpcmbuf",outpcmbuf);
fs.writeFileSync("a_ifft_out.LPCM16.raw",outpcmbuf);

// 出力と入力がどれぐらい違っているか表示
//for(var i=0;i<len;i++) console.log("diff:",output[i]-samples[i]);

