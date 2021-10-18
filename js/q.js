let a=[1,2,3];


for(let i=0;i<a.length;i++) {
    console.log("A:",i,a[i]);
}

let b=null;

console.log("BBB:",b?.length);
for(let i=0;i<b?.length;i++) {
    console.log("B:",b[i]);
}