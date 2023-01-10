const Queue=require("bull");
const q=new Queue("qqq","redis://127.0.0.1:6379");
q.process((job,done)=>{
  console.log("PROCESS: job:",job);
  done(null,{msg: "kkkkkkk"});
});


