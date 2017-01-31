var PNG=require("png-js");
console.log(PNG);
var sol_u8a=require("./sol.js");
console.log(sol_u8a);

var out;
console.log("start..");
var png = new PNG(sol_u8a);
png.decode(function(pixels) {
    console.log("pixels",pixels.length,pixels,new Date().getTime());
    out=pixels;
});
setTimeout( function() {
console.log("out p2",out,new Date().getTime());    
}, 50)

console.log(new Date().getTime())

console.log("out p1",out,new Date().getTime());



        

