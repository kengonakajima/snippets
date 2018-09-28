<?php

$a=json_decode(file_get_contents("hoge.json"));

var_dump($a);

var_dump($a->poo);
var_dump($a->zoo);
var_dump($a->koo);