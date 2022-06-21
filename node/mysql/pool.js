var mysql      = require('mysql');

var pool = mysql.createPool({
    host     : 'localhost',
    user     : 'root',
    password : 'root'
});


pool.getConnection( function(err,conn) {
    if(err){
        console.log(err);
        return;
    }
    conn.query( "select 1+1 as solution", function(err,res) {
        console.log(err);
        console.log(res);
    });
    conn.release();
});

