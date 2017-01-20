var PNG=require("png-js");
console.log(PNG);
var sol_u8a=require("./sol.js");
console.log(sol_u8a);


var png = new PNG(sol_u8a);
png.decode(function(pixels) {
    console.log(pixels.length,pixels);
    for(var i in pixels) {

    }
        
});


