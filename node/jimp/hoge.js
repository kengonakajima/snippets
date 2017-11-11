Jimp = require("jimp");

var image = new Jimp(256,256,function(err,image) {
    console.log("image:",image.bitmap.data);
    var n=256*256;
    for(var i=0;i<n;i++) {
        var ind=i*4;
        var a=0xff;
        var r=i,g=i/3,b=i/2;
        image.bitmap.data[ind]=r;
        image.bitmap.data[ind+1]=g;
        image.bitmap.data[ind+2]=b;
        image.bitmap.data[ind+3]=a;
    }
    image.write("out.png");
});