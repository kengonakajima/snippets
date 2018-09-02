#include <google/sparse_hash_map>
#include <stdlib.h>

int main() {
    //    google::sparse_hash_map<unsigned long long, int,hasher<unsigned long long>, eq> m;

    google::sparse_hash_map<unsigned long long,int> m;

    m[4848484848]=100;
    m[48484848480]=200;
    m[484848484801]=300;        

    int n=1000000;
    for(int i=0;i<n;i++) {
        unsigned long long x=random()&0xff;
        unsigned long long y=random()&0xff;
        unsigned long long z=random()&0xff;
        unsigned long long id=x+(z<<16)+(y<<32);
        m[id]=i;
    }
}
