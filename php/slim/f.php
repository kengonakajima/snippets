<?php

require "vendor/autoload.php";
use \Slim\App;

$app = new \Slim\App();


$app->get('/', function($req,$res,$args) {
    try {
        $fn = "." . $_SERVER['REQUEST_URI'];
        $f=fopen($fn,"r");
        $content=fread($f,filesize($fn));
        return $res->withStatus(200)->write($content);
    } catch(Exception $e) {
        return $res->withStatus(500)->write("error".$e->getMessage());
    }

});


$app->run();

