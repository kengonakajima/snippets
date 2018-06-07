<?php

class X {
    public static $poo=array();

    public static function hoge($t) {
        if( !isset(X::$poo[$t])) {
            echo "ZZZZZZZZZZZZZZZZZ\n";
            X::$poo[$t]=1;
        } else {
            echo "PPPPPPPPPPPPPPPPPp\n";
        }
    }
}
    
X::hoge(1);
X::hoge(1);
