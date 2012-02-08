/*

  ふぁいるをろっくするのだ。


 */
#include <stdio.h>
#include <fcntl.h>

int main()
{

    int fd = open( "hoge" , O_RDONLY );

    if( fd < 0 ){
        fprintf( stderr , "Fucked\n");
        return 1;
    }
    
    if( flock( fd , LOCK_EX )!= 0 ){
        fprintf( stderr , "lockFucked\n");
        return 1;
    }   
    
    while(1){
        sleep(1);
        fprintf( stderr , ".\n");
    }
    
    close( fd );
}
