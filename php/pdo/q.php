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

//var_dump($res);

echo "\n";
var_dump($res[0]->b);
var_dump($res[1]->b);
var_dump($res[2]->b);





echo ORM::getLastQuery();