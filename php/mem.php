<?php

$mc=new Memcached();

$mc->addServer("localhost",11211);

$data="hello";
$t0=gettimeofday(true);
$mc->set("hoge", $data, 100 );
$t1=gettimeofday(true);
$get_data = $mc->get("hoge");
$t2=gettimeofday(true);

echo $get_data, "\n";

echo ($t2-$t1), "\n";
echo ($t1-$t0), "\n";

    