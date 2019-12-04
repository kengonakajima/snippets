#include <stdio.h>
int main() {
    FILE *fp = fopen("hoge.mpeg","rb");
    int ofs=0;
    for(int l=0;l<20000;l++) {
        printf("%08d  ", ofs);
        for(int i=0;i<16;i++) {
            fseek(fp,ofs,SEEK_SET);
            unsigned char b;
            fread(&b,1,1,fp);
            printf("%02x ", b);
            ofs++;
        }
        printf("\n");
    }
}
