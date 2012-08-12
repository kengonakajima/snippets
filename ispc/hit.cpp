#include <stdio.h>
#include <stdlib.h>
#include <math.h>


class Point
{
public:
    float x,y;
    void randomize() {
        x = (random() % 100)/100.0f;
        y = (random() % 100)/100.0f;
    }
    Point* hit(Point*points, int n) {
        for(int i=0;i<n;i++){
            Point *tgt = & points[i];
            float d=4;
            if( this->x+d > tgt->x-d && this->x-d < tgt->x+d && this->y+d > tgt->y-d && this->y-d < tgt->y+d ) {
                return tgt;
            }
        }
        return NULL;
    }
};


void hitall( Point*points, Point**hits, int n ) {
    int i;
    for(i=0;i<n;i++){
        Point *p = & points[i];
        Point *hitpt = p->hit(points,n);
        hits[i] = hitpt;
    }
}
                                                 
          

void doit(int n ) {
    Point *points = (Point*)malloc( sizeof(Point)*n);
    for(int i=0;i<n;i++){
        points[i].randomize();
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
