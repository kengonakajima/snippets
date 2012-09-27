#include "cumino.h"
inline float qlen(float minx, float miny, float maxx, float maxy ){
    return abs(maxx-minx) + abs(maxy-miny);
}
int main(){
    double st = now();
    int n = 100000000;
    float tot = 0;
    for(int i=0;i<n;i++){
        tot += len(i,i,n-i,n-i);
    }
    double et = now();
    print("tot:%f t:%f",tot, (et-st));

    st = now();
    tot=0;
    for(int i=0;i<n;i++){
        tot += qlen(i,i,n-i,n-i);
    }
    et = now();
    print("qtot:%f t:%f", tot, (et-st));
    return 0;
}
