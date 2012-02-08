#include <signal.h>
#include <stdio.h>

void
aho( int a )
{

    int i;
    for(i=0;i<10;i++){
        sleep(1);
        fprintf(stderr,"#");
    }

    exit(0);
}

main()
{
    struct sigaction s;

    s.sa_handler = aho;
    s.sa_flags = SA_NOMASK;

    sigaction( SIGTERM , &s , NULL );

    while(1){
        sleep(1);
        fprintf(stderr,".");
    }
    
}
