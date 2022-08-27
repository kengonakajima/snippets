#include <stdio.h>
#include <emscripten.h>

extern "C" {
    void sayHi();
    int add(int a,int b) ;
};

EMSCRIPTEN_KEEPALIVE
void sayHi() {
  printf("Hi!\n");
}

EMSCRIPTEN_KEEPALIVE
int daysInWeek() {
  return 7;
}
EMSCRIPTEN_KEEPALIVE
int add(int a,int b) {
    return a+b;
}
