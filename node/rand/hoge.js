var lv = 5;
var out={};

var p = 65536*2*2; // 18 bit
var n=100000000;
for(var i=0;i<n;i++){
    var l = parseInt(Math.log2(Math.random() * p) );
    if(out[l]==undefined) out[l]=1; else out[l] = out[l]+1;
}
console.log("R:",out);
