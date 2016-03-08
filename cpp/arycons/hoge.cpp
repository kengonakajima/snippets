#include <stdio.h>



class Hoge
{
public:
    static int x;
    int y;
    Hoge() : y(x++) {}
};

int Hoge::x = 1;

Hoge ary[100];

int main( int argc, char **argv ) {
    for(int i=0;i<100;i++ ) {
        printf( "%d\n", ary[i].y );
    }
    
}
