<?php

$aho = 123;
$aho = array(
    "foo" => 123,
    "baz" => 111
);

file_put_contents("php://stdout", $aho); //"zzzz!!!!zz\n");

$s=var_export($aho,true);

echo $s;

