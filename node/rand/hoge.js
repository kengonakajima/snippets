var lv = 5;
var out={};

var n=100000000;
for(var i=0;i<n;i++){
    var l = parseInt(Math.log10(Math.random() * 1000*1000*1000 ) );
    if(out[l]==undefined) out[l]=1; else out[l] = out[l]+1;
}
console.log("R:",out);
