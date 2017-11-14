var request=require("request");
var fs=require("fs");

var keyfile = process.argv[2];
if(!keyfile) {
    console.log("need 1 arg for key file path");
    process.exit(1);
}

var key = fs.readFileSync(keyfile, "utf8");

console.log("Key:",key);

function makeOption(url,postdata,mimetype) {
    var method = postdata? "POST":"GET";
    
    var opts = {
        url: url,
        method: method,
        headers: {
            "User-Agent" : "nodejs",
            "Authorization" : "bearer "+key,
            "Accept" : "application/json"
        },
    };
    if(postdata) opts.body=postdata;
    if(mimetype) opts.headers["Content-Type"]=mimetype; else opts.headers["Content-Type"]="application/json";
    
    return opts;
}

opts = makeOption('https://api.github.com/repos/kengonakajima/testdata/releases');
request( opts, function(err,res,body) {
    console.log("res.body:",res.body);

    opts = makeOption('https://uploads.github.com/repos/kengonakajima/testdata/releases/8502040/assets?name=hoge.js', "hoge();", "application/javascript" );

    request( opts, function(err,res,body) {
        console.log("res.body:",res.body);
    });

});

