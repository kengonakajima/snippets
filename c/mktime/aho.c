#include <time.h>
#include <stdio.h>
#include <stdlib.h>



int main()
{
    struct tm *tp;
    time_t t;

    time( &t );
        
    tp = localtime( &t );

    printf( "sec:%d min:%d hour:%d day:%d mon:%d year:%d\n" ,
            tp->tm_sec,
            tp->tm_min,
            tp->tm_hour,
            tp->tm_mday,
            tp->tm_mon,
            tp->tm_year );

    printf( "time_t:%u\n" , t );

    {
        time_t newtime;
        struct tm *newtp;

        
        tp->tm_sec = 59;
        tp->tm_min = 59;
        tp->tm_hour = 23;

        newtime = mktime( tp );

        printf( "new time_t : %u\n" , newtime );

        newtp = localtime( &newtime );

        printf( "sec:%d min:%d hour:%d day:%d mon:%d year:%d\n" ,
            newtp->tm_sec,
            newtp->tm_min,
            newtp->tm_hour,
            newtp->tm_mday,
            newtp->tm_mon,
            newtp->tm_year );


    }
            
    
}
