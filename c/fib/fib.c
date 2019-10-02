// フィボナッチ数
#include <stdio.h>
#include <stdlib.h>

int fib(int n)
    {
        if (n < 2) {
            return n;
        } else {
            return fib(n-1) + fib(n-2);
        }
    }

int main(int argc, char *argv[])
    {
        int n;
        n = atoi(argv[1]);
        printf("%d", fib(n));
        return 0;
    }

/*
  Example
      compile:
              gcc -O3 -o fib_c fib_c.c
                  run:
                          ./fib_c.c 39
*/
