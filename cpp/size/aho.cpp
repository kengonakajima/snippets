#include <stdio.h>

void hoge( const unsigned char text[256] ) {
    printf("%s\n",text);
}
int main(){
    unsigned char aho[10];
    hoge(aho);
}
