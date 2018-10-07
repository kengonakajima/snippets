var cp=require("child_process");

var tail=cp.spawn("tail",["-3","tail.js"]);
tail.stdout.on("data", function(data) {
    console.log("data:",data.toString("utf8"));
});
tail.on("close",function(code) {
    console.log("code:",code);
});