#include <stdio.h>
#include <stdlib.h>
#include <math.h>


struct Point
{
    float x,y;
};

void randomize(Point *p) {
    p->x = (random() % 100)/100.0f;
    p->y = (random() % 100)/100.0f;
}

Point* hit(Point *p, Point*points, int n) {
    for(int i=0;i<n;i++){
        Point *tgt = & points[i];
        float d=4;
        if( p->x+d > tgt->x-d && p->x-d < tgt->x+d && p->y+d > tgt->y-d && p->y-d < tgt->y+d ) {
            return tgt;
        }
    }
    return NULL;
}


void hitall( Point*points, Point**hits, int n ) {
    int i;
    for(i=0;i<n;i++){
        Point *p = & points[i];
        Point *hitpt = hit(p,points,n);
        hits[i] = hitpt;
    }
}
                                                 
          

void doit(int n ) {
    Point *points = (Point*)malloc( sizeof(Point)*n);
    for(int i=0;i<n;i++){
        randomize( & points[i] );
    }
    Point **hits = (Point**)malloc( sizeof(Point*)*n);
    for(int i=0;i<n;i++){
        hits[i] = NULL;
    }

    hitall( points, hits, n );
    int hitcnt=0;
    for(int i=0;i<n;i++){
        if(hits[i]){hitcnt++;}
    }
    printf("hitcnt:%d\n", hitcnt );
    free(points);
    free(hits);
}


          
int main() {
    doit(1024*1024*32);
    return 0;
}
