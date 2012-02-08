#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>

int main()
{
    int fd = open( "hoge", O_CREAT | O_RDWR );
    char buf[4];
    memset(buf,99,sizeof(buf));

    assert(fd>0);
    assert( write( fd, buf ,4 ) > 0 );
    assert( lseek( fd, 100, SEEK_SET ) == 100 );
    memset(buf,100,sizeof(buf));
    assert( write( fd, buf, 4 ) > 0 );
    close(fd);
    
}
