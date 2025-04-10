var hoge=1;

setTimeout(()=>{
  console.log("x");
},300000);

var cnt=0;
setInterval(()=>{
  cnt++;
  console.log("hoge",cnt);
},1000);
