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
    int hp;
    double dec_hp_at;
    Skeleton() : hp(100000), dec_hp_at(2) {}
    virtual bool enemyPoll( double dt ) {
        if( accum_time > dec_hp_at ) {
            hp --;
            dec_hp_at = accum_time + 2;
        }
        return true;
    }
};


int main( int argc, char **argv ) {
    // init
    int n = 100000;
    Base *top=NULL;
    for(int i=0;i<n;i++){
        Skeleton *s = new Skeleton();
        s->next = top;
        top = s;
    }

    // loop
    int ln = 1000000000;
    Base *cur = top;
    for(int i=0;i<ln;i++) {
        while(cur) {
            cur->poll(0.016);
            cur = cur->next;
        }
    }
    return 0;
}
