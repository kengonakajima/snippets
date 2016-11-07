<?php


$POST_DATA = '{ "user_id" : 12345, "receiver" : 123123, "sender" : 232323, "message" : "hogehogehogehogehogehogehogehogehogehogehogehogehogehoge", "action" : "hoge" }';

    
$curl=curl_init("http://localhost:8888/");
curl_setopt($curl,CURLOPT_POST, TRUE);

//curl_setopt($curl, CURLOPT_POSTFIELDS, http_build_query($POST_DATA));
curl_setopt($curl, CURLOPT_POSTFIELDS, $POST_DATA);


curl_setopt($curl,CURLOPT_SSL_VERIFYPEER, FALSE);  // オレオレ証明書対策
curl_setopt($curl,CURLOPT_SSL_VERIFYHOST, FALSE);  //
curl_setopt($curl,CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($curl,CURLOPT_FOLLOWLOCATION, TRUE); // Locationヘッダを追跡
$output= curl_exec($curl);


?>
