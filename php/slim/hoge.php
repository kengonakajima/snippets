<?php

require "vendor/autoload.php";
use \Slim\App;

$app = new \Slim\App();

$app->get("/", function() {
  $arr = array('a' => 1, 'b' => 2, 'c' => 3, 'd' => 4, 'e' => 5);
  echo json_encode($arr);
});

$app->run();

