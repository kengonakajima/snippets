<?php

require "vendor/autoload.php";

use \Slim\App;


$conf=[
    "settings" => [
        "displayErrorDetails" => true
    ]
];
$app = new \Slim\App($conf);
$app->config("debug",true);

$app->get('/', function($req,$res,$args) {
    try {
        $p=json_encode($req->getQueryParams());
        return $res->withStatus(200)->write($req->getMethod() . " " . $req->getUri() . " " . $p  . " " . $req->getUri()->getPath() . " " . $req->getUri()->getBasePath() . " " . $req->getUri()->getQuery() );        
    } catch(Exception $e) {
        return $res->withStatus(500)->write("error".$e->getMessage());
    }

});


$app->run();

