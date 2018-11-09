var n=10000000;
var a=new Uint16Array(n);

var t0=new Date().getTime();
for(var i=0;i<n/4;i++) {
    a[i*4]=i;
    a[i*4+1]=i+2;
    a[i*4+2]=i+4;
    a[i*4+3]=i+8;
}
var t1=new Date().getTime();
console.log("0:",t1-t0);
