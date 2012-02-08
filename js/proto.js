var sys = require("sys");

function Poo() {
    sys.print( "popopopop\n");
    this.send(1);
}


Poo.prototype.send = function (a) {
    sys.print( "ahoahoaho:"+a+"\n" );
}

var a = "aaaaaaaaaa";

Poo.apply( a, [1,2,3] );


