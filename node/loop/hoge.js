let st=new Date().getTime();
let tot=0;
for(let i=0;i<10000;i++) {
  for(let j=0;j<10000;j++) {
    tot+=i+j;
  }
}
let et=new Date().getTime();
console.log("tot:",tot,et-st);

