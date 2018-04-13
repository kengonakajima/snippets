var w=50;
var fld=new Array(w);
for(var i=0;i<w;i++) {
    fld[i]=0;
}
fld[10]=9;
fld[13]=9;
fld[14]=9;
fld[32]=9;

function sim(fld,w) {
    var ind=Math.floor( Math.random() * w );

    var w=fld[ind];
    if(w<=1)return;
    
    var lw=fld[ind-1];
    if(lw!=undefined&&lw<w-1) {
        fld[ind-1]++;
        fld[ind]--;
    }    
    var rw=fld[ind+1];
    if(rw!=undefined&&rw<w-1) {
        fld[ind+1]++;
        fld[ind]--;
    }

}

for(var i=0;i<1000;i++) {
    sim(fld,w);
}

function dump() {
    var s="";
    for(var i=0;i<w;i++) {
        s+=""+fld[i];
    }
    console.log(s);
}

dump();

