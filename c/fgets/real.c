#include <stdio.h>

char buf[10];
char *boke;
int bokesize;

main()
{
	int i;
	
	boke = (char*)malloc(10);
	bokesize = 10;

	while(1)
	{

		boke[0] = '\0';
		do {
			char *backup;
			i = strlen(fgets( buf , 10,stdin));
			
			
			backup = (char *)malloc( bokesize);
			strcpy( backup , boke );
			
			bokesize+=10;
			boke = (char *) realloc(boke , bokesize );
			boke[0] = '\0';
			strcat( boke , backup );
			strcat( boke , buf );
			free( backup);
		}while(i==9);
		
		printf("bokesize:%d boke:%s\n" , bokesize , boke );
		free(boke);
		
		boke = (char *)malloc(10);
		bokesize = 10;
	}
}

	
