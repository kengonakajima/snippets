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

// heap

function makeI8Heap(mod, i8adata) {
    var nDataBytes = i8adata.length;
    var dataPtr = mod._malloc(nDataBytes);
    var dataHeap = new Uint8Array(mod.HEAPU8.buffer,dataPtr,nDataBytes);
    dataHeap.set(new Uint8Array(i8adata.buffer));    
    return dataHeap;
}

var input_data = new Int8Array(10);
for(var i in input_data) input_data[i] = i;

var input_heap = makeI8Heap(g,input_data);
var output_heap = makeI8Heap(g,new Int8Array(10));
var outputlen_heap = makeI8Heap(g,new Int8Array([10,0,0,0]));

console.log("BOs: input:", input_heap.byteOffset, "output_heap:", output_heap.byteOffset, "outputlen_heap:", outputlen_heap.byteOffset );

for(var i=0;i<10;i++)  console.log("input_heap data :", i, " : ", g.getValue(input_heap.byteOffset+i,"i8"));
for(var i=0;i<10;i++)  console.log("output_heap data :", i, " : ", g.getValue(output_heap.byteOffset+i,"i8"));
for(var i=0;i<10;i++)  console.log("outputlen_heap data :", i, " : ", g.getValue(outputlen_heap.byteOffset+i,"i8"));


var ccc = g.cwrap("ccc","number",["number","number","number","number"]);
var r = ccc(input_heap.byteOffset,10,output_heap.byteOffset,outputlen_heap.byteOffset);
output = new Int8Array(output_heap.buffer,output_heap.byteOffset,10);
var outputlen = new Int8Array(outputlen_heap.buffer,outputlen_heap.byteOffset,4);
console.log("ccc result:",r,"output:",output,"outputlen:",outputlen);

// heap access test

