#include <signal.h>



void fuck( int a )
{
    printf( "fucking.\n" );
    
}


int main()
{
    struct sigaction s , os;


    bzero( &s , sizeof(s));
    
    s.sa_handler = fuck;

/*    s.sa_mask = */

    s.sa_flags = SA_NOMASK;           
      
    
    sigaction( SIGINT , &s , &os );

    

    while(1){
        sleep(1);
    }
   
}


