# -*- coding: utf-8 -*-
# フィボナッチ数 by Python

import sys

def fib(n):
    if n < 2 :
        return n
    else:
        return fib(n-2) + fib(n-1)

argv = int(sys.argv[1])
print(fib(argv))
