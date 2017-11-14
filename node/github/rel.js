var request=require("request");
var fs=require("fs");

var keyfile = process.argv[2];
if(!keyfile) {
    console.log("need 1 arg for key file path");
    process.exit(1);
}

var key = fs.readFileSync(keyfile, "utf8");

console.log("Key:",key);

var opts = {
    url: "https://api.github.com/graphql",
    method: "GET",
    headers: {
        "Content-Type" : "application/json",
        "User-Agent" : "nodejs",
        "Authorization" : "bearer "+key
    },
    json: true,
    form: {}
};

request( opts, function(err,res,body) {
    console.log("err:",err);
    console.log("res:",res);
    console.log("body:",body);    
});