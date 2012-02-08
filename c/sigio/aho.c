#include <signal.h>
#include <fcntl.h>
#include <stdio.h>

#include <readline/readline.h>

#define BUFFSIZE 4096

int sigflag;

void sigio_func( int );


main()
{

    int n ;
    char buff[BUFFSIZE];


    signal( SIGIO , sigio_func );

    if( fcntl( 0 , F_SETOWN , getpid() ) <  0 ){
        return 1;
    }

    if( fcntl( 0 , F_SETFL , FASYNC ) < 0 ){
        return 2;
    }

    for(;;){

        sigblock( sigmask(SIGIO) );

        printf( ">");
        while( sigflag == 0 ){
            sigpause(0);
        }
#if 0
        if( ( n = read( 0 , buff , BUFFSIZE )) > 0 ){
            if( write(1,buff,n) != n ){
                return 5;
            }
        } else {

            if( n < 0 ){
                return 8;
            } else if( n == 0 ){
                return 0;
            }
        }
#endif
        {
            char *p = readline( "" );
            printf( "<%s>\n", p );
        }
        
        sigflag = 0;

        sigsetmask(0);
    }
}


void
sigio_func(int i)
{
    sigflag = 1;
}
