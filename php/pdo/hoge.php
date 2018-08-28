<?php
$user="tempra";
$pass="tempra";
$dbh = new PDO('mysql:host=localhost;dbname=tempra_common', $user, $pass);

$res=$dbh->query("select * from hoge");
var_dump($res);

foreach($res as $row) {    
  print_r($row);
}
?>