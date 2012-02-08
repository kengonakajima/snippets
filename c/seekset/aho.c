#include <stdio.h>

int main()
{
    FILE *fp;

    fp = fopen( "aho.txt", "a+" );

    fseek( fp, 0L, SEEK_SET );

    fprintf( fp, "ahoasdf %d\n",time(NULL) );
    fclose(fp);
    
}
