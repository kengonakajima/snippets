var express=require("express");
var app=express();

var server = app.listen(11112, function(){
    console.log("Node.js is listening to PORT:" + server.address().port);
});

// 写真のサンプルデータ
var photoList = [
    {
        id: "001",
        name: "photo001.jpg",
        type: "jpg",
        dataUrl: "http://localhost:3000/data/photo001.jpg"
    },{
        id: "002",
        name: "photo002.jpg",
        type: "jpg",
        dataUrl: "http://localhost:3000/data/photo002.jpg"
    }
]


app.get("/api/photo/list", function(req, res, next){
    res.json(photoList);
    console.log(req.headers['x-forwarded-for']);
    console.log(req.headers['x-forwarded-server']);
    console.log(req.headers['x-forwarded-host']);
    console.log(req.headers['x-real-ip']);            
});
