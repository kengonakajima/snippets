var Jimp=require("jimp");
Jimp.read("sf_water_center.png", function(err,data) {
    if(err)  throw err;
    data.opacity(0.65)
        .write("aaaaa.png");
   
});