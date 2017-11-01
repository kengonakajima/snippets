var mysql      = require('mysql');

var pool = mysql.createPool({
    host     : 'localhost',
    user     : 'root',
    database : 'test',
    password : ''
});


pool.getConnection( function(err,conn) {
    if(err){
        console.log(err);
        return;
    }
    conn.query( "drop table if exists __poo");
    conn.query( "create table __poo ( id int not null primary key auto_increment, created_at datetime);");
//    conn.query( "insert into __poo set created_at=now();");
//    conn.query( "insert into __poo set created_at=now();");    
    conn.query( "select * from __poo", function(err,res) {
        console.log("KKKK",err,res);
    });

});


/*
insert: OkPacket {
  fieldCount: 0,
  affectedRows: 1,
  insertId: 1,
  serverStatus: 2,
  warningCount: 0,
  message: '',
  protocol41: true,
  changedRows: 0 }

  */