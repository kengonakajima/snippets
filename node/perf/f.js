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
function set8(a,i, a0,a1,a2,a3,a4,a5,a6,a7) {
    a[i*8]=a0;
    a[i*8+1]=a1;
    a[i*8+2]=a2;
    a[i*8+3]=a3;
    a[i*8+4]=a4;
    a[i*8+5]=a5;
    a[i*8+6]=a6;
    a[i*8+7]=a7;    
}
for(var i=0;i<n/8;i++) {
    set8(a,i, i,i+2,i+4,i+8, i+10,i+12,i+14,i+16);
}
var t4=new Date().getTime();
console.log("3:",t4-t3);

