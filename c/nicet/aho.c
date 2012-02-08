#include <stdio.h>

int main()
{

    int i;
    for(;;){
        i++;
        if( ( i % 10000000 ) == 0 ) fprintf( stderr , "." );
    }
    
}
