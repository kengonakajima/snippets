var dv=new DataView(new ArrayBuffer(10));

dv.setFloat32(0,-2.345,true);
console.log("dv:",new Uint8Array(dv.buffer));
var f=dv.getFloat32(0,true);
console.log("f:",f, Math.abs(f - -2.345)<0.00001);

dv.setFloat64(0,-2.345,true);
console.log("dv:",new Uint8Array(dv.buffer));
var f=dv.getFloat64(0,true);
console.log("f:",f, Math.abs(f - -2.345)<0.00001);