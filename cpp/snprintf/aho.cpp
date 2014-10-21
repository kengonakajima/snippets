#include <stdio.h>

int main() {
    char hoge[10];
    snprintf( hoge,sizeof(hoge),"%s", "askdjfalksjdhflkajshdflkjashdlkfjahslkdjfhaklsjdfh" );
    printf(hoge);
    printf("\n");

    char poo[1000];
    snprintf(poo,sizeof(poo), "%10s", "0123456789abcdef");
    printf(poo);
    printf("\n");

    char foo[4];
    snprintf( foo, sizeof(foo), "%d", 123238 );
    printf(foo);
    printf("\n");    
}
