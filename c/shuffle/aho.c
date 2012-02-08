#include <stdlib.h>
#include <stdio.h>

void
shuffle_ints( int num, int *a, int t )
{
    int i;
    for(i=0;i<t; i++ ){
        int x = random() % num;
        int y = random() % num;
        int s;
        s = a[x];
        a[x] = a[y];
        a[y] = s;
    }
}

int
main()
{

    int aho[3]={4,5,7};

    shuffle_ints( 3, aho ,20 );

    printf( "%d %d %d\n", aho[0], aho[1], aho[2] );
}
