<?php
$user="root";
$pass="";
// ln -s /tmp/mysql.sock /var/mysql/mysql.sock
$dbh = new PDO('mysql:host=localhost;dbname=test', $user, $pass);
foreach($dbh->query("select * from hoge") as $row) {
  print_r($row);
}
?>