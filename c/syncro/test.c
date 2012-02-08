#include <stdio.h>



#include "syn.h"



#define SESSION_ID 1000

int i = 808;


int main( int argc , char **argv )
{
    int total , now;
    int i;
    
	initSyncro( atoi( argv[1]) , SESSION_ID , atoi(argv[1]));

    for(i=2;i<argc;i+=2){
        addSyncroAddr( argv[i] , atoi( argv[i+1])  );
    }
    
    

/*	addSyncroData( "i" , &i , 1 , "int" , 0 , 0 );*/

    while(1){
        
        negotiateSyncroHosts( &total , &now );

        fprintf( stderr, "total:%d now:%d\n" , total , now );
        usleep( 100*1000);
    }

    return 0;
}


