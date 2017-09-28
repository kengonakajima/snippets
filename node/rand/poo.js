var n=10000000;


for(var lv=1;lv<=18;lv++) {
    var out={};
    for(var i=0;i<n;i++){
        var maxr = Math.pow(2,lv);
        var l = parseInt(Math.log2(Math.random() * maxr));
        if(l<0)l=0;
        if(out[l]==undefined) out[l]=1; else out[l] = out[l]+1;
    }
    console.log("R:",out);        
}

