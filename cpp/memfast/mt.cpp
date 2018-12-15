#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
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

size_t SZ=1024*1024*1024;
int N=1024*65536;
int NUMTHREAD=1;

char *g_buf;

void *executor(void*arg) {
    double st=now();
    int *n=(int*)arg;
    fprintf(stderr,"executor start:%d\n",*n);

    int local_n=N/NUMTHREAD;
    for(int i=0;i<local_n;i++) {
        int at=irange(0,SZ);
        g_buf[at]++;
    }
    double et=now();
    fprintf(stderr,"executor finished:%d elt:%.3f\n",*n,et-st);
    return NULL;
}
int main() {

    g_buf=(char*)malloc(SZ);
    memset(g_buf,0,SZ);
    fprintf(stderr,"memset finished\n");
            
    pthread_t thread_ids[10];
    pthread_attr_t thread_attrs[10];
    int args[10]={0,1,2,3,4,5,6,7,8,9};

    for(int i=0;i<NUMTHREAD;i++) {
        pthread_attr_init( &thread_attrs[i]);
        pthread_attr_setscope(&thread_attrs[i],PTHREAD_SCOPE_SYSTEM );
        pthread_create(&thread_ids[i],&thread_attrs[i], executor, (void*)&args[i]);
    }
    sleep(4);
    fprintf(stderr,"main finished. buf:%d %d %d %d",
            g_buf[0],g_buf[100],g_buf[10000],g_buf[100000]);
    return 0;
}
