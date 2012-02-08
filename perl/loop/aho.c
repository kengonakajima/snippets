#include <stdio.h>

char hoge[1000000+1];

main()
{
    
    int i;
    FILE *fp;

    fp=fopen( "aho" , "w" );
    strcpy( hoge, "to be written in\n" );
    printf( "start\n" );
    for(i=0;i<100000;i++){
        fwrite( hoge , strlen( hoge ) , 1 , fp );
    }
    fclose(fp);
    printf( "end\n" );    
}
