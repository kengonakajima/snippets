<?php

$a=123;

$hoge = function($p=0) {
    //    global $a;
    
    echo "ZZZZ:",$p,":",$a,"\n";
};


$hoge(8);