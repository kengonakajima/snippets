#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int fd = open( argv[1],  O_RDONLY );
    char buf[4];
    memset(buf,99,sizeof(buf));

    int r;
    r=lseek( fd, 1024*1024*512L, SEEK_SET );
    printf("sk %d\n",r);
    r=read( fd, buf, 4 );
    printf("read %d\n",r);
    r=lseek( fd, 1024*1024*1524*2L, SEEK_SET );
    printf("sk %d\n",r);
    r=read( fd, buf, 4 );
    printf("read %d\n",r);    
    return 0;
}
