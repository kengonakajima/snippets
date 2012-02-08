#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
int
main()
{
    FILE*fp;

    if( fork() == 0 ){
        /* child */
        freopen( "hoge", "a", stderr );
        fprintf( stderr, "aho\n" );

        if( execl( "./forever.csh", NULL )<0){
            fprintf(stdout,"asdkfjhasdklfjha\n" );
        }

        return 0;
    }
    return 0;
}
