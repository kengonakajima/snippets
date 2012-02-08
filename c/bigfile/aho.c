#include <stdio.h>

int
main(int argc, char *argv )
{

    char buf[10000];
    FILE *fp = fopen( "hoge" , "w" );
    int i;

    memset( buf, 1, sizeof(buf));

    for(i=0;i<10;i++){
        fseek( fp, i*1024*1024, SEEK_SET );
        fwrite( buf, sizeof(buf),1, fp );
        fclose(fp);
    }
}
