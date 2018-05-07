<?php

class Z {
    public static function cho() {
        require_once "s2.php";
        P::kk();
    }
    public static function poo() {
        require_once "s2.php";
        P::kk();        
    }
};




Z::cho();
Z::poo();


P::kk();