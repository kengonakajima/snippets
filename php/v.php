<?php

require __DIR__ . "/vendor/autoload.php";

use Respect\Validation\Validator as v;

// 数字かどうかチェック
if (v::numeric()->validate(123)) {
  echo "Input Success !!\n";
} else {
  echo "Input Failed !!\n";
}
// 文字列かどうか
if (v::stringType()->validate(1)) {
  echo "Input Success !!\n";
} else {
  echo "Input Failed !!\n";
}