<?php

$bin=pack("nNN", 1,2,3);

$f=fopen("bin","wb");
fwrite($f,$bin,2+4+4);

//result:
//[@mbp2013 php]$ od -tx1 bin
//0000000    00  01  00  00  00  02  00  00  00  03                        
//0000012
