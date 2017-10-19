var express=require("express");
var session=require("express-session");

var app = express();


app.use( session({
     // 必須項目（署名を行うために使います）
    secret : 'my-special-secret',
    // 推奨項目（セッション内容に変更がない場合にも保存する場合にはtrue）
    resave : false,
    // 推奨項目（新規にセッションを生成して何も代入されていなくても値を入れる場合にはtrue）
    saveUninitialized : true,
    // アクセスの度に、有効期限を伸ばす場合にはtrue
    rolling : true,
    // クッキー名（デフォルトでは「connect.sid」）
    name : 'my-special-site-cookie',
    // 一般的なCookie指定
    // デフォルトは「{ path: '/', httpOnly: true, secure: false, maxAge: null }」
    cookie            : {
        // 生存期間（単位：ミリ秒）
        maxAge : 1000 * 60 * 60 * 24 * 30, // 30日
    }
}));



app.use( express.static("poo"));


var server = app.listen(3000, function(){
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

// 写真リストを取得するAPI
app.get("/", function(req, res, next){
    res.sendFile(process.env.PWD+"/index.html");
});

app.get("/api/photo/list", function(req, res, next){
    var user=req.session.user || { prevAccess:null, pv:1 };
    var prevAccess = user.prevAccess;
    var pv  = user.pv;
    user.pv += 1;
    user.prevAccess = new Date();
    req.session.user=user;

    var out= { photoList: photoList, pv:pv, prevAccess:prevAccess };
    console.log("out:",out);
    res.json(out);
});
