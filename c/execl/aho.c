#include <stdio.h>

main()
{

	int ps[2];
	char buf[10];
	FILE *fp;
	void *i;
	
	pipe( ps );
	
	if( fork() == 0 ){
		sprintf(fname ,"tmp%d" , getpid());
		execl("lasm" , "lasm" , fname,NULL );
		/* not reached */
	}

	close( ps[1]);

	fp = fdopen( ps[0] , "r" );

	while( i=fgets( buf , 1000 , fp  ) ){
		fprintf(stderr,"%d: '%s'" , i , buf);
	}

}
