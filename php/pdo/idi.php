<?php
require_once 'idiorm.php';

ORM::configure('mysql:host=localhost;dbname=test', null,"conn1");
ORM::configure('username', 'root', "conn1");
ORM::configure('password', 'monobit', "conn1");

ORM::configure('mysql:host=localhost;dbname=test2', null,"conn2");
ORM::configure('username', 'root', "conn2");
ORM::configure('password', 'monobit', "conn2");

ORM::configure('mysql:host=localhost;dbname=test3', null,"conn3");
ORM::configure('username', 'root', "conn3");
ORM::configure('password', 'monobit', "conn3");



date_default_timezone_set( "Asia/Tokyo");

echo "start\n";


$val=[ "id"=>191 ];

$a=ORM::forTable("aho","conn1")->create();
$a->set($val);
$a->save();

$a=ORM::forTable("aho2","conn2")->create();
$a->set($val);
$a->save();

$a=ORM::forTable("aho3","conn3")->create();
$a->set($val);
$a->save();




