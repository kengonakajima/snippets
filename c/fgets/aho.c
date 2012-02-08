#include <stdio.h>

char buf[10];
char *boke;
int bokesize;

main()
{
	boke = NULL;
	

	while(1)
	{
		while(1){
		        int l = strlen(fgets( buf , 10,stdin)) ;
			
			char *backup;
			backup = (char *)malloc( bokesize);
			strcpy( backup , boke );
			
			bokesize+=10;
			boke = (char *) realloc(boke , bokesize );
			boke[0] = '\0';
			strcat( boke , backup );
			strcat( boke , buf );
			free( backup);
			
			if( l == 10-1) break;
		}
		printf("bokesize:%d boke:%s\n" , bokesize , boke );
	}
}

	
