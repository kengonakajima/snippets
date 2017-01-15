#include <stdio.h>

extern "C" {
    
    int poo(int i ) {
        return i+999;
    };

    void zzz(int *out ) {
        *out=100;
    }
    int iii(int *out, int n ) {
        for(int i=0;i<n;i++) out[i]*=2;
        return 12345;
    }
    int ccc(char *in, int inlen, char *out, int *outlen  ) {
        *outlen = inlen;
        for(int i=0;i<inlen;i++) out[i] = in[i]*2+1;
        return 2;
    }
}
