
#include <stdio.h>

int main()
{
    FILE *fp = fopen( "aho.c","r");
    int ret;
    extern int errno;
    char buf[100];

    if(!fp)return 1;
    ret = fseek( fp, 123948, SEEK_SET );
    printf( "ret=%d e=%s\n", ret, strerror(errno) );

    ret = fread( buf, 1, sizeof(buf),fp);
    printf( "read ret=%d e=%s\n", ret, strerror(errno) );
    ret = fseek( fp, 12, SEEK_SET );
    printf( "ret=%d e=%s\n", ret, strerror(errno) );
    ret = fread( buf, 1, sizeof(buf),fp);
    printf( "read ret=%d e=%s\n", ret, strerror(errno) );            
    
}
