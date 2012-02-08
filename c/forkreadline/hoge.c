/*
  pipe をつかうなり


 */






#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>

#define FDOPEN 

int
main( int argc, char **argv )
{
    int a[2];       /* main command stream */
    int b[2];       /* sub return stream */
    
    pipe( a );
    pipe( b );

    if( fork() == 0 ){
        /* I am child */
        char buf[10];
#ifdef FDOPEN        
        FILE *wf , *rf;
        rf = fdopen( a[0] , "r" );
        if( rf == NULL ){
            fprintf( stderr , "cccccccccccarrrrr\n ");
            return 1;
        }
        wf = fdopen( b[1] , "a" );
        if( wf == NULL ){
            fprintf( stderr , "cccccccccccawwwwwwwwwwwwww\n ");
            return 1;            
        }
#endif
        
        while(1){
#ifdef FDOPEN
            fgets( buf, sizeof( buf) , rf );

            fprintf( wf , "adfasdf\n" );
            fflush(wf);
            fprintf( stdout, "%s\n", buf );
            fflush(stdout);
            
#else
            read( a[0] , buf, sizeof(buf) );
            fprintf( stderr, "%c %c\n", buf[0] , buf[1] );
            write( b[1] , "a" , 1 );
#endif
            
        }
    } else {
        /* I am parent */
#ifdef FDOPEN        
        FILE *wf ,*rf;

        wf = fdopen( a[1] , "a" );
        if( wf == NULL ){
            fprintf( stderr , "asdfasdfasdfa\n ");
            return 1;
        }
        rf = fdopen( b[0] , "r" );
        if( rf == NULL ){
            fprintf( stderr , "arrrrrrrrrrrrrrrrrrrrrr\n ");
            return 1;
        }
#endif        
        while(1){
            char buf[1];
            char *p = readline( "aho>" );
            if( p ==NULL)break;


#ifdef FDOPEN
            fprintf( wf , "%s\n" , p );
            fflush(wf);
            fgets( buf, sizeof( buf), rf);            
#else
            write( a[1] , p , strlen(p) );
            read( b[0], buf, 1 );
#endif
            
        }
    }
}

