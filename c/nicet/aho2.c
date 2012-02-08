#include <stdio.h>

int main()
{

    int i;
    for(;;){
        i++;
        usleep(10);
        if( ( i % 10 ) == 0 ) fprintf( stderr , "#" );
    }
    
}
