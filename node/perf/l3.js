var n=1000;
var data=new Uint8Array(32*32*32);
for(var i=0;i<data.length;i++) data[i]=i&0xff;

var t0=new Date().getTime();
var tot=0;
for(var i=0;i<n;i++) {
    var sz=32;
    for(var y=0;y<sz;y++) {
        for(var z=0;z<sz;z++) {
            for(var x=0;x<sz;x++) {
                tot+=data[x+y*sz+z*sz*sz];
            }
        }
    }
}
var t1=new Date().getTime();
console.log("tot:",tot,t1-t0);