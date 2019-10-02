<?php

/**
 * フィボナッチ数だよ
 * @param int $n
 * @return int
 */
function fib($n) {
    if ($n < 2) {
        return $n;
    } else {
        return fib($n-2) + fib($n-1);
    }
}

print fib($argv[1]);
