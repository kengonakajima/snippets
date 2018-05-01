Jimp = require("jimp");


var image = new Jimp(8,2048,function(err,image) {
    for(var y=0;y<2048;y++) {
        for(var x=0;x<8;x++) {
            var i=x+y*8;
            var ind=i*4;
            var a;
            if(y>1024) a=0xff; else if(y>(1024-128)) { var d=y-(1024-128); a=d*2; if(a>0xff)a=0xff;}
            image.bitmap.data[ind]=0;
            image.bitmap.data[ind+1]=0;
            image.bitmap.data[ind+2]=0;
            image.bitmap.data[ind+3]=a;
        }
    }


        
    image.write("out.png");
});