#include <stdio.h>
#include <math.h>
#include <stdlib.h>

class Vec2 {
public:
    float x,y;
    Vec2() : x(0),y(0){}
};

class Prop {
public:
    int id;
    Vec2 loc;
    Prop()  {
        loc.x = random();
        loc.y = random();
    }
#if 0
    bool hit( Prop *tgt, float sz ) {
        if( abs( loc.x - tgt->loc.x ) < sz && abs(loc.y - tgt->loc.y ) < sz ) {
            return true;
        } else {
            return false;
        }
    }
#endif    
#if 1
    bool hit( Prop *tgt, float sz ) {
        float l = sqrt( (loc.x-tgt->loc.x) * (loc.x-tgt->loc.x)  + (loc.y-tgt->loc.y) * (loc.y-tgt->loc.y)  );
        if( l < sz ) {
            return true;
        } else {
            return false;
        }
    }
#endif

#if 0
    bool hit( Prop *tgt, float sz ) {
        float l2 =  (loc.x-tgt->loc.x) * (loc.x-tgt->loc.x)  + (loc.y-tgt->loc.y) * (loc.y-tgt->loc.y)  ;
        if( l2 < sz*sz ) {
            return true;
        } else {
            return false;
        }
    }
#endif

#if 0
    bool hit( Prop *tgt, float sz ) {
        sz/=2.0;
        if( ( loc.x + sz > tgt->loc.x - sz ) &&
            ( loc.x - sz < tgt->loc.x + sz ) &&
            ( loc.y + sz > tgt->loc.y - sz ) &&
            ( loc.y - sz < tgt->loc.y + sz ) ) {
            return true;
        } else {
            return false;
        }
    }
#endif    
};

#define N 10000

Prop *props[N];

int countHit() {
    int hitcnt = 0;
    for(int i=0;i<N;i++)  {
        Prop *p = props[i];
        bool hit = false;
        for(int j=0;j<N;j++) {
            Prop *phit = props[j];
            if( p != phit && p->hit(phit,20000000) ) hit = true;
        }
        if( hit ) hitcnt ++;
    }
    return hitcnt;
}

int main( int argc, char **argv ) {
    printf("start\n");
    for(int i=0;i<N;i++ ) {
        props[i] = new Prop();
    }

    for( int i=0;i<10;i++) {
        int hit = countHit();
        printf("hit:%d\n", hit );
    }
}
