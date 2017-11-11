const fs=require("fs");
fs.readdirSync("../somewhere_not_exist").forEach( function(f) {
    console.log("f:",f);
});