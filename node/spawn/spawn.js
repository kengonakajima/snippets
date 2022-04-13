const { spawn } = require('child_process');
const cmd = "ls *js | grep js";

const arr = cmd.trim().split(" ");
const arr0 = arr.shift();
const p = spawn(arr0, arr, {
  shell: true, // NOTE: cat aaa.txt | grep "foo" のようなshellのパイプ "|" を処理できるようにする
});
p.on('exit', function(code) {
  console.log("exit:", code);
});
p.on('error', function(err) {
  console.log("error:", err);
});
p.stdout.setEncoding('utf-8');
p.stdout.on('data', function(data) {
    const outary=data.split("\n");
    console.log("stdout:", JSON.stringify(outary));
});
p.stderr.setEncoding('utf-8');
p.stderr.on('data', function(data) {
  console.log("stderr:", data);
});
