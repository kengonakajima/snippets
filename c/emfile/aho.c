#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    int i;
    
    for( i=0;;i++){
        int fd = open( "a.out", O_RDONLY );
        fprintf(stderr,".");
        if( fd < 0 ){
            fprintf(stderr, "n:%d errno:%d(%d)\n", i, errno, EMFILE );
            break;
            
        }
    }
}
