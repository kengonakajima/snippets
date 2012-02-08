#include <stdio.h>

void func( int &a )
{
        a++;
}

int main()
{
        int b = 10;

        func( b );

        printf( "%d\n", b );
}
