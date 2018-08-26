var n=1024*1024;
var loop=100;

function now() { return new Date().getTime(); }

var ary=new Array(n);
for(var i=0;i<n;i++) ary[i]=0;


function incr_with_obj(o) {
    ary[o.i0]++;
    ary[o.i1]++;
    ary[o.i2]++;    
}
function incr_with_arg(i0,i1,i2) {
    ary[i0]++;
    ary[i1]++;
    ary[i2]++;
}
function incr_with_ary(a) {
    ary[a[0]]++;
    ary[a[1]]++;
    ary[a[2]]++;    
}
var t0=now();
for(var i=0;i<loop;i++) {
    for(var j=0;j<n;j++) incr_with_obj({i0:j,i1:j,j2:j});
}
var t1=now();
for(var i=0;i<loop;i++) {
    for(var j=0;j<n;j++) incr_with_arg(j,j,j);
}
var t2=now();
for(var i=0;i<loop;i++) {
    for(var j=0;j<n;j++) incr_with_ary([j,j,j]);
}
var t3=now();


console.log("obj:",t1-t0, "arg:",t2-t1,"ary:",t3-t2);
