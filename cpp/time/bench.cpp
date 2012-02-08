#include <iostream>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>



int main()
{

    int i;
    for(i=0;i<10000000;i++){
        struct timeval tmv;
        gettimeofday( &tmv, NULL );
        if( tmv.tv_usec == 0 ){
            std::cerr << "zero." << std::endl;
        }
    }

    return 0;    
}

