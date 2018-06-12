<?php

$a=50000000000;

$b=$a & 0xffffffff;
$c=$a >> 32;

var_dump($a);
var_dump($b);
var_dump($c);

$d=($c<<32) + $b;

var_dump($d);