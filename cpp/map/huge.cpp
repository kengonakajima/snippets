#include <unordered_map>
#include <stdlib.h>
#include <stdint.h>

int main() {
    std::unordered_map<uint64_t,bool> idm;
    int n=1000000*20;
    for(int i=0;i<n;i++) {
        uint64_t id = rand();
        idm[id]=true;
    }
    printf("size:%lu\n",idm.size());
}
