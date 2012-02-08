#include <stdio.h>
#include <fcntl.h>


struct aho
{
	int a;
	char *b;
};

int main()
{
	struct aho a;
	FILE *fp;
	
	a.a = 100;
	a.b = NULL;
	a.b = (char *)malloc( 10 );


	fp = fopen( "fuck" , "w");
	if( fp == NULL ){
		perror( "fuck");
		exit(1);
	}

	a.b[0] = 'a';
	a.b[1] = 'a';
	a.b[2] = 'a';
	a.b[3] = 'a';
	a.b[4] = 'a';
	a.b[5] = 'a';
	a.b[6] = 'a';
	a.b[7] = 'a';
	a.b[8] = 'a';
	a.b[8] = '\0';
	
	fwrite( (char *)&a , 4+10 ,1, fp);
	fclose( fp );
	
}
