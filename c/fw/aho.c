#include <stdio.h>


int main()
{
    FILE *fp = fopen( "aho.t" , "w" );

    fprintf( fp , "asdjfasdfasdkfaksdjfaksjdfkasdf\n" );

    fseek( fp , 0 , SEEK_SET );

    fprintf( fp , "qqqqqqqq\n" );

    fclose(fp);
    
}
