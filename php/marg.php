<?php

function hoge($a) {
    var_dump(func_get_args());
}

hoge(1,2);
