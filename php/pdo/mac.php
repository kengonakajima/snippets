<?php
$user="root";
$pass="";
$dbh = new PDO('mysql:host=localhost;dbname=test', $user, $pass);
foreach($dbh->query("select * from hoge") as $row) {
  print_r($row);
}
?>