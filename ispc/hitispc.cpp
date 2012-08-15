#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hit_ispc.h"
using namespace ispc;


void randomize(struct Point *p) {
    p->x = (random() % 100)/100.0f;
    p->y = (random() % 100)/100.0f;
}
          

void doit(int n ) {
    Point *points = (Point*)malloc( sizeof(Point)*n);
    for(int i=0;i<n;i++){
        randomize( &points[i] );
    }

    int nhit = hitcnt( points, n );
    
    printf("hitcnt:%d\n", nhit );
    free(points);
}


          
int main() {
    doit(16); //024);//*1024);//*32);
    return 0;
}
