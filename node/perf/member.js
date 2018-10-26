var n=10000000;
var a=new Array(n);
var b={a:a};

var t0=new Date().getTime();
for(var i=0;i<n;i++) {
    a[i]=i;
}
var t1=new Date().getTime();
for(var i=0;i<n;i++) {
    b.a[i]=i;
}
var t2=new Date().getTime();
for(var i=0;i<n;i++) {
    a[i]=i;
}
var t3=new Date().getTime();
console.log("T10:",t1-t0, "T21:",t2-t1, "T32:",t3-t2);