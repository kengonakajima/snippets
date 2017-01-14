var a = require("./f.js")
console.log("a",a);

var x=a.ccall("hoge", "void", ["number","string"], [10,"hoooo"]);
console.log("x",x);

