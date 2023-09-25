const ffi = require('ffi-napi');
const ref = require('ref-napi');
const ArrayType = require('ref-array-napi');



const int = ref.types.int;

const myLibrary = ffi.Library('./hoge.dylib', {
  'add': [int, [int, int]]
});

const result = myLibrary.add(2, 3);
console.log(`2 + 3 = ${result}`);

let tot=0;
const st=new Date();
const n=100000;
for(let i=0;i<n;i++) tot+=myLibrary.add(i,1);
const et=new Date();

const dt=(et-st);

console.log("tot:",tot,"time:",(dt/n));

