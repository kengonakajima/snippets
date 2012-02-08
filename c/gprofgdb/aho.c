#include <stdio.h>

int
main( int argc, char **argv )
{
    int i;
    char hoge[] = "ahoaho";
    char nasu[] = "ahoaho";
    
    for(i=0;i<10000000;){
        if( strcmp( hoge, nasu ) == 0 ) i++;
    }
    return 0;
}
