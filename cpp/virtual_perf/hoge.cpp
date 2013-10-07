#include <stdio.h>

class Base {
public:
    double accum_time;
    Base *next;
    Base() : accum_time(0), next(NULL) {}
    virtual bool basePoll(double dt){ return true;}
    virtual bool poll(double dt) {
        accum_time += dt;
        if( basePoll(dt) == false ) return false;
        return true;
    };
};

class Char : public Base {
public:
    virtual bool charPoll( double dt ) { return true; }
    virtual bool basePoll( double dt ) {
        if( charPoll(dt) == false ) return false;
        return true;
    }
};

class Mob : public Char {
public:
    virtual bool mobPoll( double dt ) { return true; }
    virtual bool charPoll( double dt ) {
        if( mobPoll(dt) == false ) return false;
        return true;
    }
};
class Enemy : public Mob {
public:
    virtual bool enemyPoll( double dt ) { return true; }
    virtual bool mobPoll( double dt ) {
        if( enemyPoll(dt) == false ) return false;
        return true;
    }
};
class Skeleton : public Enemy {
public:
    int cnt;
    Skeleton() : cnt(0) {}
    virtual bool enemyPoll( double dt ) {
        cnt++;
        return true;
    }
};


int main( int argc, char **argv ) {
    // init
    int n = 10000;
    Base *top=NULL;
    for(int i=0;i<n;i++){
        Skeleton *s = new Skeleton();
        s->next = top;
        top = s;
    }

    // loop
    for(int i=0;i<100000;i++) {
        Base *cur = top;
        while(cur) {
            cur->poll(0.016);
            cur = cur->next;
        }
    }

    // fin
    double tot=0;
    Skeleton *s= (Skeleton*)top;
    while(s) {
        tot += s->cnt; 
        s = (Skeleton*)s->next;
    }
    printf("tot:%f",tot);
    
    return 0;
}
