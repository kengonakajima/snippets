const mysql = require('mysql2/promise');
const pool = mysql.createPool({host:'localhost', user: 'root', password:'root', database: 'guardians'});



async function hoge() {
    const [rows,fields] = await pool.execute("select * from fuga where name=?",["zoo"]);
    console.log(rows);
    //console.log(fields);
}

async function get(table,id) {
    const [rows,fields] = await pool.execute("select * from fuga where id=1",[]);
    console.log("rows:",rows);
}

get("fuga",2);
