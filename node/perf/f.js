var n=10000000;
var a=new Float32Array(n);

var t0=new Date().getTime();
for(var i=0;i<n/4;i++) {
    a[i*4]=i;
    a[i*4+1]=i+2;
    a[i*4+2]=i+4;
    a[i*4+3]=i+8;
}
var t1=new Date().getTime();
console.log("0:",t1-t0);

function set4(a,i,x,y,z,w) {
    a[i*4]=x;
    a[i*4+1]=y;
    a[i*4+2]=z;
    a[i*4+3]=w;
}

for(var i=0;i<n/4;i++) {
    set4(a,i, i, i+2, i+4, i+8 );
}
var t2=new Date().getTime();
console.log("1:",t2-t1);
for(var i=0;i<n/4;i++) {
    a[i*4]=i;
    a[i*4+1]=i+2;
    a[i*4+2]=i+4;
    a[i*4+3]=i+8;
}
var t3=new Date().getTime();


console.log("2:",t3-t2);