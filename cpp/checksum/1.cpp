//
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

uint16_t sum1comp16(const char *b, int32_t l) {
    uint16_t cur=0;
    for(int i=0;i<l;i+=2) {
        uint16_t v=b[i] | (b[i+1]<<8);
        cur += (~v);
    }
    if((l%2)>0) {
        uint16_t v=b[l-1];
        cur += (~v);
    }    
    return cur;    
}
int main() {
    char hoge[]="hoge fuga";
    uint16_t s=sum1comp16(hoge,strlen(hoge));
    printf("sum1comp16: %04x cksum:%04hx plus:%04x\n",s,(uint16_t)~s, (uint16_t)(s+(~s)));
    char bad[]="hogf fuga";
    uint16_t t=sum1comp16(bad,strlen(bad));
    printf("sum1comp16: %04x cksum:%04hx plus:%04x\n",t,(uint16_t)~s, (uint16_t)(t+(~s)));
    return 0;
}
