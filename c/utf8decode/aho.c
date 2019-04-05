#include <stdio.h>
#include <string.h>
#include "utf8_decode.h"

int main() {
    char *s=u8"あいう";
    utf8_decode_init(s,strlen(s));

    for(int i=0;;i++) {
        int u=utf8_decode_next();
        printf("[%d]: %x", i, u);
        if(u==0xffffffff)break;
    }
    return 0;
}
