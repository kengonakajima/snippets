

eval( "function fff() { console.log('ffff'); }");

fff();

var fs=require("fs");
var js=fs.readFileSync("func.js").toString();
console.log("read js:",js);

eval(js);

f();