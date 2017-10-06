var sqlite3 = require("sqlite3");

sqlite3.verbose();

var db = new sqlite3.Database(':memory:');
db.serialize();
db.on('error', function (err) {
    console.error(err);
    process.exit(1);
});

for(var kk=0;kk<1000;kk++){
    db.run('CREATE TABLE IF NOT EXISTS sample (info TEXT)');
    for (var i = 0; i < 10; i++) {
        db.run('INSERT INTO sample VALUES (?)', 'i = ' + i);
    }

    db.each('SELECT rowid AS id, info FROM sample', function (err, row) {
        console.log(row.id + ': ' + row.info);
    });


    db.close();
}

