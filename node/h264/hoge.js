const fs=require("fs");
const HME = require("h264-mp4-encoder");

var n=200;
var w=256, h=192;

var t0=new Date().getTime();

var raw=[];
for(var i=0;i<n;i++) {
    var v= (i*7)%256;
    raw.push(new Uint8Array(w * h * 4).fill(v));;
}
var t1=new Date().getTime();
console.log("gen done:",(t1-t0));

HME.createH264MP4Encoder().then(encoder => {
    // Must be a multiple of 2.
    encoder.width = w;
    encoder.height = h;
    console.log("enc:",encoder.outputFilename);
    encoder.initialize();
    // Add a single gray frame, the alpha is ignored.
    for(var i=0;i<n;i++) {
        encoder.addFrameRgba(raw[i]);
    }
    // For canvas:
    // encoder.addFrameRgba(ctx.getImageData(0, 0, encoder.width * encoder.height).data);
    encoder.finalize();
    const uint8Array = encoder.FS.readFile(encoder.outputFilename);
    console.log(uint8Array);
    fs.writeFileSync("poo.mp4",uint8Array );
    encoder.delete();
    var t2=new Date().getTime();    
    console.log("enc done:",(t2-t1));    
})

