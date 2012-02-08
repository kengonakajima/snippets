#include <signal.h>



int
main()
{
    struct sigaction s , os;

    bzero( &s , sizeof(s));
    s.sa_handler = SIG_IGN;
    s.sa_flags = SA_NOMASK;
    sigaction( SIGINT , &s , &os );
    while(1){
        sleep(1);
        printf( "aho\n" );
    }
}
