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
        int loopc = *outlen;
        *outlen = inlen;
        for(int i=0;i<inlen&&i<loopc;i++) out[i] = in[i]*2+1;
        return 2;
    }
    int sss(const char* input, size_t input_length, char* compressed, size_t* compressed_length ) {
        int hoge = *compressed_length / 2;
        for(int i=0;i<input_length&&i<hoge;i++) {
            compressed[i] = input[i]*2+1;
        }
        *compressed_length = hoge;
        return 0;
    }
    int sizeofsizet() {
        return sizeof(size_t);
    }
    
}
