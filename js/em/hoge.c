#include <stdio.h>


int hoge(int i ) {
    return i+2;
}

int lll(const char *s) {
    int n=0;
    for(int i=0;;i++) {
        if(s[i]==0)break;
        n++;
    }
    return n;
}
