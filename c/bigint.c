#include <inttypes.h>
#include <stdio.h>

int main(){
    uint64_t x = 1415360018354868224;
    printf("%lld\n%lld\n",x,(uint64_t)65536*(uint64_t)65536*(uint64_t)30000*(uint64_t)30000);
    return 0;
}
