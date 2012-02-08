#include <stdio.h>
main()
{
	FILE *fp , *fp2,*fp3,*fp4;

	char aho[10],aho2[10];

	fp  = fopen("/dev/ttyp2","rw");
	fp2 = fopen("/dev/ttyp2" , "rw");
	fp3 = fopen("/dev/ttyp7" , "w");
	fp4 = fopen("/dev/ttyp7" , "w");
	while(1){
		fgets( aho,1, fp );
		fprintf(fp3, "A:%c\n" , aho[0] );
		fgets( aho2 , 1,fp2);
		fprintf(fp4, "B:%c\n" , aho2[0] );
		sleep(1);
	}
	
}
