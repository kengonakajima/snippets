#include <stdio.h>

void hoge( int &i )
{
    i+=1;
}

int
main()
{
    int a,b=0;
    hoge(b);
    hoge(b);    

    printf( "%d\n",b);
}
