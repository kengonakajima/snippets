<?php
 $res = curl_init("http://yahoo.com/");
 print($res);
 curl_exec($res);
 curl_close($res);
?>
