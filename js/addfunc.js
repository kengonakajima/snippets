


var sys = require("sys");

function SString(s) {
    this.data = s;
}

var ss = new SString("askdjf");

SString.prototype.send = function(x) {
    sys.print( "t:" + this.data+":"+x+"\n");
}


ss.send(1);


