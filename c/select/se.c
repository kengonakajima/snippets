#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>

main( int argc , char **argv )
{
	static struct timeval timeout;
	int i;
	
	

	for(i = 0;;i++){
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;//1*1000;//1000 * 100;
        
		if( select( 0 , (fd_set *) 0 , (fd_set *)0, (fd_set *)0 , &timeout )<0){
			fprintf(stderr,"select error.\n");
	
			exit(1);
		}

        fprintf(stderr,"#" ); 
		if(i % 20 == 0 ){
			fprintf(stderr,"." );
        }
	}
}

	
