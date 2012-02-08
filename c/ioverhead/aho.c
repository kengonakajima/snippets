#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>


int main( int argc, char **argv )
{
    int cnt=0;
    int fd = open( "a", O_RDONLY );
    if(fd<0){
        printf("%s\n",strerror(errno));
        return 1;
    }
    char buf[10000];
    int n = atoi( argv[1] );
    while(1){
        int r = read( fd, buf, n );
        if( r<n)break;
        int i;
        for(i=0;i<n;i++){
            if( buf[i] == '\n' )cnt++;
        }
    }
    printf("%d\n",cnt);
}
