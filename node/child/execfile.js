var cp=require("child_process");

cp.execFile("gzip",["x"], function(e,stdout,stderr) {
    console.log("finished:",stdout,stderr);
    cp.execFile("ls",["-l"], function(e,stdout,stderr) {
        console.log("finished:",stdout,stderr);
    });
});