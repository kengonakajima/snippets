function H() {
    console.log("function H");
}
H.prototype.constructor = H;
H.prototype.hoge = function() {
    console.log("H.hoge");
}

function G() {
    H.call(this);
    console.log("function G");
}
G.prototype = Object.create(H.prototype);
G.prototype.constructor = G;
G.prototype.hoge2 =  function() {
    console.log("G.hoge2");
}

var hh = new H();
var gg = new G();


hh.hoge();
gg.hoge();
gg.hoge2();