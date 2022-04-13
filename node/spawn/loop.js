const { spawn } = require('child_process');
const cmd = "./a.out";

function call(cmd) {
    const arr = cmd.trim().split(" ");
    const arr0 = arr.shift();
    const p = spawn(arr0, arr, {shell: false});
    p.on('error', function(err) {
        console.log("error:", err);
    });
    p.stdout.setEncoding('utf-8');
    p.stdout.on('data', function(data) {
        const outary=data.split("\n");
        console.log("stdout:", JSON.stringify(outary));
    });
}

for(let i=0;i<100;i++) {
    var st=new Date().getTime();
    call(cmd);
    var et=new Date().getTime();
    console.log("T:",et-st);
}



