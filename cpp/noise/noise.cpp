#include <stdio.h>
#include <math.h>

#include <stdlib.h>

inline double max( double a, double b ){
    return  (a>b) ? a:b;
}
inline double min( double a, double b ){
    return (a<b) ? a:b;
}


inline double range( double a, double b ) {
    long r = rand();
    double rate = (double)r / (double)(0x7fffffff);
    double _a = min(a,b);
    double _b = max(a,b);
    return _a + (_b-_a)*rate;
}



float noise(int x) {
    x = (x<<13) ^ x;
    return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

int main() {
    float x0=0,x1=0,x2=0;
    float dx0=0.1,dx1=0.1,dx2=0.1;
    for(int i=0;i<100;i++) {
        x0 += dx0;
        x1 += dx1;
        x2 += dx2;
        dx0 += range(-0.01,0.01);
        dx1 += range(-0.01,0.01);
        dx2 += range(-0.01,0.01);        
        if(dx0<0)dx0*=-1;
        if(dx1<0)dx1*=-1;
        if(dx2<0)dx2*=-1;                
        
        float y0 = sin(x0);
        float y1 = sin( x1 * 2.7 + 8.8 );
        float y2 = sin( x2 * 8.31 - 4.8 );
         
        int out = (1+y0) * 20 + (1+y1) * 10 + (1+y2) * 4;
        for(int i=0;i<out;i++) printf(" ");
        printf("*\n");
    }
}
