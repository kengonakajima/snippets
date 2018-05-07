<?php
require_once 'idiorm.php';

ORM::configure('mysql:host=localhost;dbname=test');
ORM::configure('username', 'root');
ORM::configure('password', 'monobit');
date_default_timezone_set( "Asia/Tokyo");

echo "start\n";

$val=[ "id"=>191 ];

$a=ORM::forTable("aho")->where("id",191)->find_many();

var_dump( (int)$a[0]->id);



$a=ORM::forTable("aho")->where("id",9191)->find_many();

var_dump( $a);
var_dump( $a[0]);

var_dump(NULL);

var_dump(NULL==null);

$a=NULL;

var_dump($a==NULL);
