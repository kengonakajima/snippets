<?php

require "vendor/autoload.php";
use \Slim\App;

$app = new \Slim\App();

$app->get("/", function() {
  echo "app root!";
});

$app->run();

