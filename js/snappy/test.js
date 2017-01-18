var SnappyJS = require("./snappyjs.min.js")

console.log("SN",SnappyCompressor, SnappyDecompressor );



var orig=new Uint8Array([11,22,33,44,55,11,12,12,12,13,13,13,13,13,13,34,34,34,34,43,23,3,25,4,4,44,4,4,4,44,4,4,4,4,44,4,4,4,4,4,44,4,4,4,4,4,44,4,4,4,4,4,44,4,4,4,4,4,4,44,4,4,4,4,4,4,44,4,4,4,4,4,4,44,4,4,44,44,4,44,4,4,4,4,4,4,4,4,4,4,44,4,4,4,23,234,23,42,2,3,3,3,3,3,3,23,3,3]);

var comp = new SnappyCompressor(orig);
var outbuf = new Uint8Array(1000);
var outpos = comp.compressToBuffer(outbuf);
console.log("outpos:",outpos, "outbuf:", outbuf);
var decomp = new SnappyDecompressor(new Uint8Array(outbuf,0,outpos) );
var deoutbuf = new Uint8Array(1000);
var deoutpos = decomp.uncompressToBuffer(deoutbuf);
console.log("deoutpos:",deoutpos,"deoutbuf:",deoutbuf);

for(var i=0;i<deoutpos;i++) {
    if( deoutbuf[i] != orig[i] ) console.log("NO match at ", i);
}
