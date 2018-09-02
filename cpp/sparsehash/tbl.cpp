#include <stdlib.h>
#include <stdio.h>
int tbl[128*128*128];

int main() {
    int n=100000000;
    for(int i=0;i<n;i++) {
        unsigned long long x=random()&0x7f;
        unsigned long long y=random()&0x7f;
        unsigned long long z=random()&0x7f;
        unsigned long long id=x+(z<<7)+(y<<14);
        tbl[id]=random()&0xffff;
    }
    //    for(int i=0;i<1000;i++) printf("%d ", tbl[i]);
    printf("set done\n");
    int found=0;
    for(int i=0;i<n;i++) {
        unsigned long long x=random()&0x7f;
        unsigned long long y=random()&0x7f;
        unsigned long long z=random()&0x7f;
        unsigned long long id=x+(z<<7)+(y<<14);
        if(tbl[id]==(random()&0xffff))found++;
    }
    printf("get done, found:%d\n",found);    
    return 0;
}
