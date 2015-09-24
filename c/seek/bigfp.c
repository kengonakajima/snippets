#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
    for(int i=0;;i++) {
        
        FILE *fp = fopen( "hoge", "wb" );
        assert(fp);
    
        char buf[4] = {0,1,2,3};

        int n = 400 * 1000 * 1000;
        int r;
        fseek( fp, n, SEEK_SET );
        fwrite( buf,  1, 4, fp );
        fclose(fp);

        fprintf( stderr, "loop:%d\n",i );
    }    
}
