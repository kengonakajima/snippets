#include <signal.h>



int main()
{
    sigaction( SIGINT , 0, 0 );

    

    while(1){
        sleep(1);
    }
   
}


