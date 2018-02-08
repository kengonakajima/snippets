<?php

require "vendor/autoload.php";
use \Slim\App;

$app = new \Slim\App();


$app->get('/[{path:.*}]', function($req,$res,$args) {
    try {
        $uri=$req->getUri();
        $fn="./" . $uri->getPath();
        $f=fopen($fn,"r");
        $content=fread($f,filesize($fn));
        return $res->withStatus(200)->write($content);
    } catch(Exception $e) {
        return $res->withStatus(500)->write("error".$e->getMessage());
    }

});


$app->run();

