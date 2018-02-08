<?php

require "vendor/autoload.php";

use \Slim\App;


$conf=[
    "settings" => [
        "displayErrorDetails" => true
    ]
];
$app = new \Slim\App($conf);


$app->map(["get","post"], '/', function($req,$res,$args) {
    return $res->withStatus(200)->write("RRR");
});

$app->map(["get","post"], '/a', function($req,$res,$args) {
    return $res->withStatus(200)->write("a");
});

$app->map(["get","post"], '/a/b', function($req,$res,$args) {
    return $res->withStatus(200)->write("ab");
});

$app->run();
