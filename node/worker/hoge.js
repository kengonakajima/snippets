const { Worker, isMainThread, workerData } = require('worker_threads');

let hoge=0;


if(isMainThread) {
    console.log("mainthr", __filename);

    for (let i = 0; i < 4; i++) {
        new Worker(__filename, { workerData: i });
  }

    
} else {
    console.log("worker", workerData);
    var tot=0;
    for(var i=0;i<1000000;i++) {
        for(var j=0;j<1000;j++) tot+=j;
    }
    
}