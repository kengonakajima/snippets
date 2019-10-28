#include <stdlib.h>
#include <stdint.h>
#include <bitset>

// osm node id: 7G~8G 

int main() {
    std::bitset<1024*1024*32> bs;

    for(int j=0;j<100;j++) {
        int n=10000*10000;
        for(int i=0;i<n;i++) {
            uint64_t id = rand();
            int ind=id%(1024*1024*32);
            bs[ind]=1;
        }
        printf("%d\n",j);
    }
    printf("size:%lu\n",bs.size());
}
