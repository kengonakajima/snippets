const Readable=require("stream").Readable;
const Speaker=require("speaker");

const sine=new Readable();
sine.t=0;
sine._read = function(n) {
    var sampleNum = n/2;
    var u8ary = new Uint8Array(n);
    var dv=new DataView(u8ary.buffer);
    for(var i=0;i<sampleNum;i++) {
        this.t += 0.01;
        dv.setInt16(i*2,Math.sin(this.t)*20000,true);
    }
    this.push(u8ary);
}

const spk=new Speaker({
    channels: 1,
    bitDepth: 16,
    sampleRate: 48000,
    samplesPerFrame: 512    
});

sine.pipe(spk);

