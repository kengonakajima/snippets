<?php

$a="asdasd8,";
$b=json_decode($a);

echo $b;


echo "----\n";

$s=json_encode( array(1,2));
echo $s,"\n";
$b=json_decode($s,true);


echo var_dump($b);