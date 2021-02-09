#include <stdio.h>

#define NAME(a,b) a ## b

void fuga_hoge() {
    printf("fuga\n");
}
void piyo_hoge() {
    printf("piyo\n");
}

int main() {
    NAME( fuga, _hoge )();
    NAME( piyo, _hoge )();    
    return 0;
}
