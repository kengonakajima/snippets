var brrito = require("burrito");

var fs =require("fs");

var f = fs.openSync( "/Users/ringo/lightspeed/sv/js/ps.js", "r" );
var bytes = fs.readSync( f, 999999, 0, "ascii" );
console.log(bytes);

