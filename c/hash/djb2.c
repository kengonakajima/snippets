#include <stdio.h>
#include <stdint.h>
unsigned int djb2(char *word, int wordlen) {
    unsigned int hashAddress=5381;
    for(int i=0;i<wordlen;i++) {
        hashAddress= ((hashAddress<<5)+hashAddress)+word[wordlen];
    }
    return hashAddress;
}

uint32_t adler32(const void *buf, size_t buflength) {
     const uint8_t *buffer = (const uint8_t*)buf;

     uint32_t s1 = 1;
     uint32_t s2 = 0;

     for (size_t n = 0; n < buflength; n++) {
        s1 = (s1 + buffer[n]) % 65521;
        s2 = (s2 + s1) % 65521;
     }     
     return (s2 << 16) | s1;
}
uint32_t adler32_wrap16(const void *buf, size_t buflength) {
    return (adler32(buf,buflength))&0xffff;
}
int main() {
    for(int i=0;i<1000;i++) {
        char s[100];
        sprintf(s,"%d",i);
        //        unsigned int h=djb2(&val,4);
        unsigned int h=adler32(s,strlen(s));
        printf("%u\n",h & 0xf);
    }
}
