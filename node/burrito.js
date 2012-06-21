
/*
var burrito = require('burrito');

var src = burrito('f() && g(h())\nfoo()', function (node) {
                      if (node.name === 'call') node.wrap('qqq(%s)');
                  });

console.log(src);
*/

var burrito = require("burrito");

var fs =require("fs");


fs.readFile( "./jquery.js", function(err,data) {
                 console.log("data:",data);
                 var res = burrito(data, function(node) {
                                       console.log("NODE:",node.name );
                                       if( node.name == "call" ) node.wrap( "AHO(%s)" );
                                   });

                 console.log("res:",res);
                 
             });

