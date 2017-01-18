var sn = require("snappyjs");
console.log(sn);
var u8buf = new Uint8Array([1,2,3,1,2,3,1,2,3,1]);
console.log(u8buf);
var compressed = sn.compress(u8buf.buffer);
console.log(compressed);
var compu8 = new Uint8Array(compressed);
console.log(compu8);
var decompressed = sn.uncompress(compu8.buffer);
console.log(decompressed);
var decompu8 = new Uint8Array(decompressed);
console.log(decompu8);

