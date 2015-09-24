#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
    for(int i=0;;i++) {
        int fd = open( "hoge", O_CREAT | O_RDWR );

        char buf[4] = {0,1,2,3};

        int n = 400 * 1000 * 1000;
        int r;
        r = lseek( fd, n, SEEK_SET );
        assert(r==n);
    
        r = write( fd, buf ,4 );
        assert(r==4);
    
        r = close(fd);
        assert(r==0);
        if(i%1000==0)        fprintf( stderr, "loop:%d\n", i );
    }
    
}
