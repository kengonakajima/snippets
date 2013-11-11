#include <stdio.h>

class Prop {
public:
    Prop() { printf( "Prop ctor\n" ); };
    ~Prop() { printf( "Prop dtor\n" ); };
};

class Char : public Prop {
public:
    Char() { printf( "Char ctor\n" ); }
    ~Char() { printf( "Char dtor\n" ); }
};

class Prop2 {
public:
    Prop2() { printf( "Prop2 ctor\n" ); };
    virtual ~Prop2() { printf( "Prop2 dtor\n" ); };
};

class Char2 : public Prop2 {
public:
    Char2() { printf( "Char2 ctor\n" ); }
    ~Char2() { printf( "Char2 dtor\n" ); }
};


int main( int argc, char **argv ) {

    Prop *p = new Prop();
    delete p;

    Char *c = new Char();
    delete c;

    printf("-------\n");

    Char *c_ = new Char();
    Prop *p_ = (Prop*) c_;
    delete p;

    printf("-------\n");
    
    Char2 *c2 = new Char2();
    Prop2 *p2 = (Prop2*) c2;
    delete p2;
    
    
}
