const fs=require("fs");
const HME = require("h264-mp4-encoder");
HME.createH264MP4Encoder().then(encoder => {
    // Must be a multiple of 2.
    encoder.width = 256;
    encoder.height = 192;
    console.log("enc:",encoder.outputFilename);
    encoder.initialize();
    // Add a single gray frame, the alpha is ignored.
    for(var i=0;i<200;i++) {
        var v= (i*7)%256;
        encoder.addFrameRgba(new Uint8Array(encoder.width * encoder.height * 4).fill(v));
    }

    // For canvas:
    // encoder.addFrameRgba(ctx.getImageData(0, 0, encoder.width * encoder.height).data);
    encoder.finalize();
    const uint8Array = encoder.FS.readFile(encoder.outputFilename);
    console.log(uint8Array);
    fs.writeFileSync("poo.mp4",uint8Array );
    encoder.delete();
})
