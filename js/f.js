var sys = require('sys');

function x(a,b,c){
    sys.print(a+":"+b+":"+c+":"+this+"\n");
}

var s = "assd";

x.apply( s, [1,2,3,4] );


