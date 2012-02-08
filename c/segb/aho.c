#include<stdio.h>



int a;
int aaa[1000000];

#define elementof(x) ( sizeof(x) / sizeof(x[0]))

int main()
{

    char *buf;


    buf = (char*) malloc( 1000 * 1000 * 1000  );


    printf( "%d\n" , buf );


    buf[0] = 'a';
    
    
    
}
