var f = require("./f.js")
console.log("f",f);

console.log("hoge", f.ccall("hoge", "number", ["number"], [10]) )
console.log("lll", f.ccall("lll", "number", ["string"], ["aaaaaa"]) )

var g = require("./g.js")
console.log("g",g);

console.log("poo", g.ccall("poo", "number", ["number"], [111]) )
