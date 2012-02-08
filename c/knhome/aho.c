/*

  suCheckPassword test

 */

#define _KND_SERVER_        /* readme.txt にあった */

#include <stdio.h>
#include <time.h>

#include "h/KnConst.h"
#include "h/KnLendef.h"

#define ptime(x)  { time_t t;time(&t); printf( x "%s" , ctime( &t));}

int suCheckPassword(char* mem_id, char* password);


int main()
{
    int ret;

    ptime( "(id,pas)=(aaa,bbb) " ) ;
    ret = suCheckPassword( "aaa" , "bbb" );
    printf( "RET:%d\n", ret );

    ptime( "(id,pas)=(Zaaa,bbb) " ) ;
    ret = suCheckPassword( "Zaaa" , "bbb" );
    printf( "RET:%d\n", ret );

    ptime( "(id,pas)=(aaa,Zbbb) " ) ;
    ret = suCheckPassword( "aaa" , "Zbbb" );
    printf( "RET:%d\n", ret );

    ptime( "(id,pas)=(Zaaa,Zbbb) " ) ;
    ret = suCheckPassword( "Zaaa" , "Zbbb" );
    printf( "RET:%d\n", ret );    
    
    ptime( "Finished " );

    return 0;
}
