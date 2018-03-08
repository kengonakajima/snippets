<?php
$a=(int)0xfffffff0;
if($a>0x7fffffff) $a-=0x100000000;
echo $a;