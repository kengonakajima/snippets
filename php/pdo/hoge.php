<?php
$user="mwf_test";
$pass="mwf_test";
$dbh = new PDO('mysql:host=localhost;dbname=mwf_dev_common', $user, $pass);
foreach($dbh->query("select * from hoge") as $row) {
  print_r($row);
}
?>