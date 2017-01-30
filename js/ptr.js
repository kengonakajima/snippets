a=[];

function P(x) {
    this.x=x;
}

var p = new P(111);
var q = new P(222);

a[0]=p;
a[1]=q;
a[2]=p;

console.log( a[0]==p, a[0]==a[2]);