/*

 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

int main() {
    char buf[100];
    memset( buf, 0xcd, sizeof(buf));
    
    FILE *fp = fopen( "aho.dat", "r+");

    assert( fseek(fp, 1000, SEEK_SET ) == 0 );
    assert( fwrite( buf, 1, sizeof(buf), fp ) == 100 );
    assert( fseek( fp, 0, SEEK_SET ) == 0 );

    int frr = fread( buf, 1, 100, fp );
    fprintf(stderr,"frr:%d errno:%d\n",frr,errno);
    assert( frr == 100 );

    int i;
    
    for(i=0;i<100;i++){
        fprintf( stderr, "d:%d\n", buf[i] );
    }
    fclose(fp);
    return 0;
}
