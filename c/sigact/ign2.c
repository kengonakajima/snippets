#include <signal.h>



int main()
{
    sigignore( SIGINT );

    while(1){
        sleep(1);
    }
   
}


