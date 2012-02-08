
#include "unpack.h"
#include "lssproto_cli.h"


int  main()
{
    lssproto_InitClient( NULL , 16384 ,1 );

    lssproto_TK_send( 1 , "こうしゃべったのだ。" ,  5 );

    

    
}

void lssproto_Echo_recv( int fd , char *in )
{
    
}
