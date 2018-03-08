<?php

$bin=pack("nNN", 1,2,3);

$f=fopen("bin","wb");
fwrite($f,$bin,2+4+4);

//result:
//[@mbp2013 php]$ od -tx1 bin
//0000000    00  01  00  00  00  02  00  00  00  03                        
//0000012
$pk=pack("NNnnNQNN", 1,-2,3,-4,-100,6,7,8 );
var_dump( $pk );
$out=unpack("Na/Nb/nc/nd/Nres/Qf/Ng/Nh", $pk);
$result=$out["res"];
if($result>0x7fffffff) $result-=0x100000000;

echo $result, "\n";
var_dump( $out );
