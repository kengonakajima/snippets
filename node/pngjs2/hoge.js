console.log("Buffer:",Buffer);
var PNG=require("pngjs2").PNG;
console.log("Buffer:",Buffer);
console.log(PNG);
var sol_u8a=require("./sol.js");
console.log(sol_u8a);
var buf = new Buffer(sol_u8a);
console.log(buf);
var out = new PNG.sync.read(buf);
console.log("output:",out);



        

