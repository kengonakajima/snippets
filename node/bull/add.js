const Queue=require("bull");
const q=new Queue("qqq","redis://127.0.0.1:6379");
q.add( { info: "hogehogehoge"});
console.log("added");


