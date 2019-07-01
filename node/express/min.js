var express=require("express");  // npm in express
var app = express();
var server = app.listen(3000, function(){
    console.log("Node.js is listening to PORT:" + server.address().port);
});
app.get("/hoge", function(req, res, next){
    console.log("req:",req);
    res.send("ok");
});
