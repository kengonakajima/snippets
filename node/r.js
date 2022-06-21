const {irange}=require("./util.js");

let r0ind=0, r1ind=0,r2ind=0;

for(let i=0;i<1000000;i++) {
    const r0 = Math.random()*2 - 1;
    const r1 = Math.random()*2 - 1;
    const r2 = Math.random()*2 - 1;
    const rr=r0*r1/2+0.5;
    const rrr=r0*r1*r2/2+0.5;    
    let out=irange(0,1000,Math.abs(rrr));
    if(out<10) console.log("out:",i,out,rr,r0,r1);
    
}
