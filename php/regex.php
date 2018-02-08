<?php

$a="/app.php/AssetBundle/x64";

preg_match( "/" . preg_quote('/AssetBundle/(.*)', '/') . "/", $a, $matches, PREG_OFFSET_CAPTURE );

print_r($matches);