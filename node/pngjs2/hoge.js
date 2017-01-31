var PNG=require("pngjs2").PNG;
console.log(PNG);
var sol_u8a=require("./sol.js");
console.log(sol_u8a);
var buf = new Buffer(sol_u8a);
var out = new PNG.sync.read(buf);
console.log("output:",out);



        

