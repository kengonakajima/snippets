#include <stdio.h>

class Char {
public:
    virtual void onattack( int a, int b ) {
        printf("ch a:%d b:%d\n",a,b);
    }
    void onattack( int a ) {
        onattack( a, 1000 );
    }
};

class Enemy : public Char {
public:
    virtual void onattack( int a, int b ) {
        printf("e a:%d b:%d\n", a, b);
    }
};

int main() {
    Char *ch = new Char();

    ch->onattack(1,2);
    ch->onattack(3);

    Enemy *e = new Enemy();
    e->onattack(1,2);
    e->Char::onattack(4);    
    
}
