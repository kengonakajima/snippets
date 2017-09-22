var w=1024*4;
var h=1024*4;
var n = w*h;
var ary=[];

function Cell() {
    this.t = 0;
    this.tt = 100;
    this.ttt = 200;
    this.a = 1;
    this.b = 2;
}
Cell.prototype.increment = function(v) {
    this.t += v;
}
console.log("init:", new Date());

for(var i=0;i<n;i++){
    ary.push( new Cell());
}

for(var j=0;j<100;j++) {
    console.log("increment:", j, new Date());
    for(var i=0;i<n;i++){
        ary[i].increment(i+j);
        ary[i].a += 2;
        ary[i].b += ary[i].a;        
    }
}

var tot=0;
for(var i=0;i<n;i++){
    tot+=ary[i].t;
}
console.log("tot:",tot);