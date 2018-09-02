#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/time.h>

inline double now() {
    struct timeval tmv;
    gettimeofday( &tmv, NULL );
    return tmv.tv_sec  + (double)(tmv.tv_usec) / 1000000.0f;
}


int main() {
    std::unordered_map<uint64_t,int> m;
    
    int n = 1000000;

    double t0=now();
    int chkn=1000000;

    int hoge=0;
    for(int i=0;i<chkn;i++) {
        uint64_t x=random()&0xff;
        uint64_t y=random()&0xff;
        uint64_t z=random()&0xff;
        uint64_t id=x+(z<<16)+(y<<32);        
        if(id==58559)hoge++;
    }
    double t1=now();
    printf("hoge: %f\n",t1-t0);
    for(int i=0;i<chkn;i++) {
        uint64_t x=random()&0xff;
        uint64_t y=random()&0xff;
        uint64_t z=random()&0xff;
        uint64_t id=x+(z<<16)+(y<<32);
        m[id]=i;
    }
    double t2=now();    
    printf("asho:%f\n",t2-t1);
    int found=0;
    for(int i=0;i<chkn;i++) {
        uint64_t x=random()&0xff;
        uint64_t y=random()&0xff;
        uint64_t z=random()&0xff;
        uint64_t id=x+(z<<16)+(y<<32);
        if(m[id]>0)found++;
    }
    double t3=now();        
    printf("f:%d %f\n",found,t3-t2);
        
}
