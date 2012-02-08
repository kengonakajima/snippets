
#include <stdio.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>

void
alrm( int i )
{

    sleep(1);
    printf( "%d\n" , i );

    
//    kill( getpid() , SIGALRM );
}

int
main( int argc , char **argv )
{
    char *p;


    static struct sigaction as,aos;
   
    bzero( &as, sizeof( as ));


    as.sa_handler = alrm;
    

    sigaction( SIGALRM, &as, &aos );

    
    kill( getpid() , SIGALRM );
    
    while(1){
        fprintf( stderr, "fucking\n" );
        p = readline( "aho>" );

        printf( "fuck: %s\n", p );
    }
    
}
