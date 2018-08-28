<?php

$mc=new Memcached();

$mc->addServer("localhost",11211);
$mc->set("hogekey", "helloval",100 );

$get_data = $mc->get("hogekey");

echo "get data:", $get_data, "\n";

$mc->set("app.localfugakey", "fugaval", 1000 );
$mc->set("app.localpiyokey", "piyoval", 1000 );

sleep(2);
$keys=$mc->getAllKeys();

var_dump($keys);

$env = "app.local";
$keys = preg_grep("/^{$env}/",$keys);
$ret=array();
if(!empty($keys)) {
    foreach($keys as $k) {
        $ret[]=substr($k,strlen($env));
    }
}
var_dump($ret);


    