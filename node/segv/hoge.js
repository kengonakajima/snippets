chp=require("child_process");

/*
var hoge=chp.spawn("./crash");

hoge.stdout.on("data", (data) => {
    console.log("stdout:",data) ;
});

hoge.on("close", (code) => {
    console.log("coooodde:",code);
})  ;

*/

var gdb=chp.spawn("lldb",["./crash"]);

gdb.stdout.on("data", (data) => {
    var msg=data.toString("utf8");
    console.log("stdout:",msg);
    if(msg=="crashing!") {
        gdb.stdin.write("bt\n");
        gdb.stdin.write("l\n");            
    }

});

gdb.on("close", (code) => {
    console.log("coooodde:",code);
})  ;

gdb.stdin.write("r\n");

