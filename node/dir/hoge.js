const fs=require("fs");
fs.readdirSync("..").forEach( function(f) {
    console.log("f:",f);
});