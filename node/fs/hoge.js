const fs=require("fs");

var fd=fs.openSync("hoge_js.txt","w");

console.log(fd);


for(var i=0;i<1000000;i++) {
    fs.writeSync(fd, "hoge"+i+"\n");
    fs.fdatasync(fd);
}
fs.closeSync(fd);
