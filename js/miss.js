var sys = require("sys");

var a={}

a.aho = "aho";
a["hoge"]="hohoho";

a.fuga = function(xx) { sys.print ("aaaaaaaa:"+xx+"\n"); }

sys.print( a.aho + ":" + a.hoge + "\n")

a.fuga(1);


var k = {
    __noSuchMethod__ : function( id,args){ sys.print( "aksjdfakljsdhf\n" ); }    
};

k.aaaaa(1);
