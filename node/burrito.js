
/*
var burrito = require('burrito');

var src = burrito('f() && g(h())\nfoo()', function (node) {
                      if (node.name === 'call') node.wrap('qqq(%s)');
                  });

console.log(src);
*/

var burrito = require("burrito");

var fs =require("fs");

function spaces(depth) {
    var out="";
    for(var i=0;i<depth;i++){
        out += " ";
    }
    return out;
}

function traverse(ast,depth) {
    for(var i=0;i<ast.length;i++){
        var node = ast[i];
        if( typeof(node)== "object" ){
            traverse(node,depth+1);
        } else {
            if( node.length == undefined ){
                console.log( spaces(depth), "NODE:", node );
            } else {
                console.log( spaces(depth), "NODEN:", node.length );
                traverse(node,depth+1);
            }
        }
    }
}

fs.readFile( "./t.js", function(err,data) {
                 console.log("data:",data);
                 var ast = burrito.parse(data.toString(),false,true);
         //        traverse(ast,0);
                 console.log(JSON.stringify(ast,null,1));

                 
                 var res = burrito(data, function(node) {
                                       console.log("NODE:",node.name, "VAL:", node.value, "PAR:", node.parent() );
                                       if( node.name == "call" ) node.wrap( "AHO(%s)" );
                                   });
                 console.log("res:",res);
                 
             });

