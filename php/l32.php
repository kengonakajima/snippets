<?php

function binary_dump($in){                                                                                 
    $bindata        = unpack("C*", $in );                                                                  
    $ret            = "";                                                                                  
    foreach($bindata as $v){                                                                               
        $ret    .= sprintf("%02x ",$v);                                                                    
    }                                                                                                      
    return $ret;                                                                                           
}                                                                                                          


$a=0x1;

$b=$a >>32;
$c=$a & 0xffffffff;

var_dump($a,$b,$c);

$data="hoge";
$l=strlen($data);
$bytes = pack( "VVa".$l, $b,$c, $data );
/*
                $bytes=pack(
                            "VVvvVVVV",
                            $l-4, // LEN 32bit LE
                            0x1, // seq 32bit LE
                            0x0, // opt 16bit LE
                            0x0101, // RPC_ID_CREATE_ROOM
                            $room_id_u32, // 32bit LE
                            $room_id_l32, // 32bit LE                           
*/

echo binary_dump($bytes), "\n";