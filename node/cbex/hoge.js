// exception in a callback
var fs=require("fs");
    
try {
//    fs.readFileSync("pppp");
    console.log(kkkkk);
    var cnt=0;
    setInterval( function() {
        console.log("int");
        cnt++;
        if(cnt==4) console.log(pppp);
    },1000);

} catch(e) {
    console.log("err:",e);
}
    