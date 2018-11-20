<?php
$pdo=new PDO('mysql:host=127.0.0.1;dbname=test',"root","monobit");

$user_id=$_REQUEST['user_id'];
$item_id=$_REQUEST['item_id'];

$res=$pdo->query("select * from items where id=" . $item_id );
$row=$res->fetch();

$price=$row['price'];

$res=$pdo->query("update users set wallet=wallet-" . $price . ", item0=" . $item_id . " where id=" . $user_id );

$res=$pdo->query("select wallet from users where id=" . $user_id );
$row=$res->fetch();
$wallet=$row["wallet"];

$a=array( 'result' => "success", "price" => $price, "wallet" => $wallet);

echo json_encode($a);


