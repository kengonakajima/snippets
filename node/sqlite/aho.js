var sqlite3 = require("sqlite3").verbose();


var db = new sqlite3.Database("poo");

db.serialize(function(){
    db.run("create table if not exists ents (info varchar(30))");
    db.run("insert into ents values ('aa');");
    db.each("select info from ents", function(err,row) {
        console.log("ROW:",row) ;
    });
});


