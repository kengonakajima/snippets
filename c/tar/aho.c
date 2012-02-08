#include <stdio.h>







int main()
{
    FILE *fp = fopen( "aho" , "w");
    while(1){
        usleep( 1 * 1000 );
        fprintf( fp,"%d\n", 1 );
        fflush(fp);
    }
    
}
