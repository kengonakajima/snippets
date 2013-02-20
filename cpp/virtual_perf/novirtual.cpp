#include <stdio.h>

class Base {
public:
    double accum_time;
    Base *next;
    bool (*callback)( Base *self, double dt);    
    
    bool poll( double dt ) {
        if( callback(this,dt) == false ) return false;
        return true;
    }


    Base() : accum_time(0), next(NULL), callback(NULL) {}    
};


class Char : public Base {
public:
    bool (*callback)( Char *self, double dt);
    
    static bool charPoll( Char *self, double dt ) {
        if( self->callback(self,dt) == false ) return false;
        return true;
    }
    Char() : Base() {
        Base::callback = (bool (*)(Base*,double) )charPoll;
    }
};


class Mob : public Char {
public:
    bool (*callback)( Mob *self, double dt );

    static bool mobPoll( Mob *self, double dt) {
        if( self->callback(self,dt) == false ) return false;
        return true;
    }

    Mob() : Char() {
        Char::callback = (bool (*)(Char*,double))mobPoll;
    }
};

class Enemy : public Mob {
public:
    bool (*callback)( Enemy *self, double dt );
    static bool enemyPoll( Enemy *self, double dt) {
        if( self->callback(self,dt) == false ) return false;
        return true;
    }
    Enemy() : Mob() {
        Mob::callback = (bool (*)(Mob*,double))enemyPoll;
    }
};
class Skeleton : public Enemy {
public:
    int hp;
    double dec_hp_at;
    static bool skeletonPoll( Skeleton *self, double dt ) {
        if( self->accum_time > self->dec_hp_at ) {
            self->hp --;
            self->dec_hp_at = self->accum_time + 2;
        }
        return true;
    }
    Skeleton() : Enemy(), hp(100000), dec_hp_at(2) {
        Enemy::callback = (bool (*)(Enemy*,double))skeletonPoll;
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


