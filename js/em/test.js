var f = require("./f.js")
console.log("f",f);

console.log("hoge", f.ccall("hoge", "number", ["number"], [10]) )
console.log("lll", f.ccall("lll", "number", ["string"], ["aaaaaa"]) )

var g = require("./g.js")
console.log("g",g);

console.log("poo", g.ccall("poo", "number", ["number"], [111]) )

// 
var data = new Int32Array(1);
var nDataBytes = 1 * 4;
var dataPtr = g._malloc(nDataBytes);
var dataHeap = new Uint8Array(g.HEAPU8.buffer,dataPtr,nDataBytes);
dataHeap.set(new Uint8Array(data.buffer));

var zzz = g.cwrap("zzz","number", ["number"]);
zzz( dataHeap.byteOffset );
var result = new Int32Array(dataHeap.buffer,dataHeap.byteOffset,data.length);
console.log("zzz_result got:",result);

g._free(dataHeap.byteOffset);

//

data = new Int32Array(120);
for(var i in data) data[i]=i;
nDataBytes = 120*4;
dataPtr = g._malloc(nDataBytes);
dataHeap = new Uint8Array(g.HEAPU8.buffer,dataPtr,nDataBytes);
dataHeap.set(new Uint8Array(data.buffer));

var iii = g.cwrap("iii","number",["number","number"]);
var r = iii(dataHeap.byteOffset,120);
var output = new Int32Array(dataHeap.buffer,dataHeap.byteOffset,data.length);
console.log("zzz ret:",r," out:",output );
g._free(dataHeap.byteOffset);

