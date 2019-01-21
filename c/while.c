#include <stdio.h>
#include <stdlib.h>

int main() {
    double d=0;
    long long cnt=0;
    while(1) {
        int i;
        cnt++;
        usleep(1);
        for(i=0;i<1000000;i++) {
            d+=i;
        }
        if(cnt%100==0) printf("cnt:%lld %f\n",cnt,d);
    }

}
