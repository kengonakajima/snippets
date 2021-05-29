const fs = require("fs");
const sampleRate = 48000;
const soundHz = 440;
const samplePerPeriod = sampleRate/soundHz;
const radianPerSample = Math.PI*2/samplePerPeriod;
const volume = 5000;
const bytesPerSample = 2;
const duration = 1;
const sampleNum = duration * sampleRate;
const bytes=bytesPerSample*sampleNum; // 1秒分(96000バイト)
var buf = new ArrayBuffer(bytes);
var dv = new DataView(buf);
var radian=0;
for(var i=0;i<sampleNum;i++) {
    const sample = Math.sin(radian)*volume;
    dv.setInt16(i*2, sample, true); // little endian
    console.log("hoge:",i,sample);
    radian+=radianPerSample;
}
fs.writeFile("raw_440hz.pcm16le", Buffer.from(buf), function(err) {if(err)console.log(err); });
