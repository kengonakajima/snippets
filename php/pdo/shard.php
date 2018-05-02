<?php


require_once 'idiorm.php';

ORM::configure('mysql:host=localhost;dbname=shardtest', null,"idconn");
ORM::configure('username', 'root', "idconn");
ORM::configure('password', 'monobit', "idconn");

ORM::configure('mysql:host=localhost;dbname=test', null,"conn1");
ORM::configure('username', 'root', "conn1");
ORM::configure('password', 'monobit', "conn1");

ORM::configure('mysql:host=localhost;dbname=test2', null,"conn2");
ORM::configure('username', 'root', "conn2");
ORM::configure('password', 'monobit', "conn2");

ORM::configure('mysql:host=localhost;dbname=test3', null,"conn3");
ORM::configure('username', 'root', "conn3");
ORM::configure('password', 'monobit', "conn3");


ORM::configure('logging', true, "idconn");
ORM::configure('logger', function($log_string, $query_time) {
  // $log_string にSQLクエリが、$query_timeに処理時間が入ります。
  echo $log_string . ' in ' . $query_time . "\n";
}, "idconn");




date_default_timezone_set( "Asia/Tokyo");

echo "start\n";

$db=ORM::getDb("idconn");
$db->exec("DROP TABLE IF EXISTS id_range;");
$db->exec("CREATE TABLE IF NOT EXISTS id_range ( id_min BIGINT NOT NULL, id_max BIGINT NOT NULL, shard_id INT NOT NULL, min_shard_id INT NOT NULL, max_shard_id INT NOT NULL ); ");
$db->exec("INSERT INTO id_range values ( 0, 9, 1, 1, 3 );");
$db->exec("INSERT INTO id_range values ( 10, 19, 2, 1, 3 );");
$db->exec("INSERT INTO id_range values ( 20, 29, 3, 1, 3 );");
$db->exec("INSERT INTO id_range values ( 30, 39, 1, 1, 3 );");

$db->exec("DROP TABLE IF EXISTS id_generator;");
$db->exec("CREATE TABLE IF NOT EXISTS id_generator ( id bigint NOT NULL PRIMARY KEY AUTO_INCREMENT, created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP );");

function generateNewID() {
    $id=ORM::forTable("id_generator","idconn")->create();
    $id->save();
    return $id->id();
}

function getShardId($id) {
    $shard=ORM::forTable("id_range","idconn")->where_lte("id_min",$id)->where_gte("id_max",$id)->find_one();
    //    var_dump($shard);
    return $shard["shard_id"];
}

for($i=0;$i<50;$i++) {
    $new_id=generateNewID();
    $shard_id=getShardId($new_id);
    if(!$shard_id) $shard_id="invalid";
    echo $new_id, " ", $shard_id, "\n";
}

