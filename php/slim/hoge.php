<?php

require "vendor/autoload.php";
use \Slim\App;

$app = new \Slim\App();

/*
$app->get("/hoge/:a0/:a1/:a2/:a3/:a4", function($a0,$a1,$a2,$a3,$a4) {
    echo "a0:",$a0,"\n" ;
    echo "a1:",$a1,"\n" ;
    echo "a2:",$a2,"\n" ;
    echo "a3:",$a3,"\n" ;
    echo "a4:",$a4,"\n" ;        
});

$app->get("/z/:a0", function($a0) {
  $arr = array('a0' => $a0, 'b' => 2, 'c' => 3, 'd' => 4, 'e' => 5);
  echo json_encode($arr);
});

$app->get("/:a0", function($a0) {
    $z=$app->request->get("t");
  $arr = array('a0' => $a0, 'z' => $z, 'c' => 3, 'd' => 4, 'e' => 5);
  echo json_encode($arr);
}
*/

$app->get('/', function($req,$res,$args) {
    try {
        $p=json_encode($req->getQueryParams());
        return $res->withStatus(200)->write($req->getMethod() . " " . $req->getUri() . " " . $p );        
    } catch(Exception $e) {
        return $res->withStatus(500)->write("error".$e->getMessage());
    }

});


$app->run();

