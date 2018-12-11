#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


inline double now() {
    struct timeval tmv;
    gettimeofday( &tmv, NULL );
    return tmv.tv_sec  + (double)(tmv.tv_usec) / 1000000.0f;
}

inline float maxf( float a, float b ){
    return (a>b) ? a:b;
}
inline float maxf( float a, float b, float c ) {
    return maxf( maxf(a,b), c );
}
inline float maxf( float a, float b, float c, float d ) {
    return maxf( maxf(a,b), maxf(c,d) );
}
inline double maxd( double a, double b ){
    return  (a>b) ? a:b;
}
inline double mind( double a, double b ){
    return (a<b) ? a:b;
}
inline float minf(float a, float b) {
    return (a<b) ? a:b;
}
inline int maxi( int a, int b ){
    return  (a>b) ? a:b;
}
inline int mini( int a, int b ){
    return (a<b) ? a:b;
}

inline double range( double a, double b ) {
    long r = random();
    double rate = (double)r / (double)(0x7fffffff);
    double _a = mind(a,b);
    double _b = maxd(a,b);
    return _a + (_b-_a)*rate;
}
inline int irange( int a, int b ) {
    double r = range(a,b);
    return (int)r;
}



int main() {
    size_t sz=1024*1024*1024;
    char *buf=(char*)malloc(sz);
    memset(buf,0,sz);
    fprintf(stderr, "cleared\n");
    int n=1024*1024*64;
    int skip=2048; // 8:0.32s 16:0.35s 32:0.39s 64:0.47s 128:0.73s 256:1.19s 512:1.26s 1024:1.39s 2048:1.62s
    int at=0;
    double st=now();
    for(int i=0;i<n;i++) {
        at+=irange(skip/2,skip);
        //at=irange(0,sz); // 1.92s
        buf[at%sz]++;
    }
    double et=now();
    fprintf(stderr,"buf:%d %d %d %d time:%f",buf[0],buf[100],buf[10000],buf[100000], et-st);
    return 0;
}
