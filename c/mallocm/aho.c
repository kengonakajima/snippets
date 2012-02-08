#include <stdio.h>
#include <malloc.h>

int main()
{
    int i;
    for(i=0;;i++){
        char *m;
        if( ( m = malloc( 1 ) ) == 0 )break;
        if( (i % 10000) == 0 ){ fprintf( stderr , ".");}
    }
    
}
