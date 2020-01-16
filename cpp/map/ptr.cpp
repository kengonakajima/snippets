#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

inline double now() {
    struct timeval tmv;
    gettimeofday( &tmv, NULL );
    return tmv.tv_sec  + (double)(tmv.tv_usec) / 1000000.0f;
}


class Hoge {
public:
    int a,b,c;
    int buffer[1024];
    Hoge(int a,int b,int c) : a(a),b(b),c(a+b+c) {
        buffer[a%100]=a;
        buffer[b%120]=b;        
    }
};
int main() {
    const int N=40000;
    std::unordered_map<uint64_t,Hoge*> m;
    Hoge *ary[N];
    double t0=now();
    for(int i=0;i<N;i++) {
        ary[i]=new Hoge(i,i,i);
        m[i]=ary[i];
    }
    double t1=now();
    printf("t0-1:%f\n",t1-t0);
    uint64_t tot=0;
    for(int i=0;i<10000000;i++) {
        tot+=random();
    }
    double t2=now();
    printf("t1-2:%f\n",t2-t1);
    tot=0;
    for(int i=0;i<10000000;i++) {
        int r=random();
        int id=r%N;
        Hoge *ptr = ary[id];
        tot+=ptr->c;
    }
    double t3=now();
    printf("t2-3:%f tot:%llu\n",t3-t2,tot);
    tot=0;
    for(int i=0;i<10000000;i++) {
        int r=random();
        int id=r%N;
        Hoge *ptr = m[id];
        tot+=ptr->c;
    }
    double t4=now();
    printf("t3-4:%f tot:%llu\n",t4-t3,tot); // 9 times slower!
}
