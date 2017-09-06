// 1Mbpsぐらい送る
//

var net = require("net")
var cl = new net.Socket();
cl.connect( 22222, "210.140.72.117", function() {
    console.log("connected");
    cl.write("hoge");   
});

cl.on("data", function(data) {
    console.log("data:",data);
});

cl.on("close", function() {
    console.log("closed");
});