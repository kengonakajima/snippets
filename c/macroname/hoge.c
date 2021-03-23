#include <stdio.h>

#define NAME(b) fuga ## b

void fuga_hoge() {
    printf("fuga\n");
}
void piyo_hoge() {
    printf("piyo\n");
}

int main() {
    NAME( _hoge )();
    NAME( _hoge )();    
    return 0;
}
