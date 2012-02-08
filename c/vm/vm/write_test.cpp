#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int i;
    int fd = open( "./logfile", O_RDWR );
    for(i=0;i<100000000;i++){
        size_t s = 1 + random() % 3;
        char buf[50];
        memset(buf,0,sizeof(buf));
        write( fd, buf, s );
	if((i%10000)==0) write( 2, ".", 1 );
    }
    close(fd);
}
