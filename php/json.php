<?php

$a=null;
$k=json_decode($a);

$a="asdasd8,";
$b=json_decode($a);
if($b) {
    echo "KKKKKKKKKKKKKKKKK\n";
} else {
    echo "BBBBBBBBBBBBBBBBBBb\n";
}
echo $b;


echo "----\n";

$s=json_encode( array(1,2));
echo $s,"\n";
$b=json_decode($s,true);


echo var_dump($b);