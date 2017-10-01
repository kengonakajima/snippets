var mysql      = require('mysql');



var connection = mysql.createConnection({
    host     : 'localhost',
    user     : 'root',
    password : ''
});

connection.connect( function(err) {
    console.log("co err:",err);
    connection.end();
});

