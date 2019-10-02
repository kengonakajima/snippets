<?php

$fp=fopen("hoge_php.txt","w");
for($i=0;$i<1000000;$i++) {
    fwrite($fp,"hoge:".$i."\n");
    fflush($fp);
}
fclose($fp);
