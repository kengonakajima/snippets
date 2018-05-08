<?php
require_once 'idiorm.php';

ORM::configure('mysql:host=localhost;dbname=test');
ORM::configure('username', 'root');
ORM::configure('password', 'monobit');
date_default_timezone_set( "Asia/Tokyo");

ORM::configure("logging",true);
    
echo "start\n";

$val=[ "id"=>191 ];

$a=ORM::forTable("zako")->find_many();

//var_dump($a);

echo ORM::getLastQuery();


$res=ORM::forTable("zako")->distinct()->select("b")->find_many();
echo ORM::getLastQuery();

//var_dump($res);

echo "\n";

$a=array();

foreach($res as $row) {
    array_push($a,$row->b);
}
var_dump($a);



