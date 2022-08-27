/*
  以下はぶらうざなのか
const fs = require('fs');

// https://natsutan.hatenablog.com/entry/2021/04/24/121900
const wasmBuffer = fs.readFileSync('hoge.wasm.o');
WebAssembly.instantiate(wasmBuffer).then(wasmModule => {
  // Exported function live under instance.exports
  const { aec3_init } = wasmModule.instance.exports;
  aec3_init(4,0);
  cosole.log("done");
});
*/



var hoge = require('./hoge.js');

hoge.onRuntimeInitialized = () => {
  var sayHi=hoge.cwrap("sayHi","void",[]);
  sayHi();
  var add=hoge.cwrap("add","number",["number","number"]);
  console.log(add(3,4));
}


//  instance._sayHi(); // direct calling works
//  instance.ccall("sayHi"); // using ccall etc. also work
//  console.log(instance._daysInWeek()); // values can be returned, etc.

