<?php

$sk=socket_create(AF_INET,SOCK_STREAM,0);
try{
    $ret=socket_connect($sk,"127.0.0.1",12131);
    if(!$ret) {
        $ecode=socket_last_error();
        $msg=socket_strerror($ecode);
        echo "PPPPPPPPPPPPPPPPPP" . $msg . "\n";
        socket_close($sk);
    }
}catch(\Exception $e){
    socket_close($sk);
    echo "ZZZZZZZZZZZZZZZZ\n";
}