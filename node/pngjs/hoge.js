var Jimp=require("jimp");

Jimp.read("a.png").then( function(a) {
    console.log("a:",a);
    Jimp.read("b.png").then( function(b) {
        console.log("b:",b);
        a.composite(b,0,0)
            .write("out.png");        
    });
})