#include <stdio.h>


main()
{
	FILE *fp;
	char buf[256];
	int i;
	for(i=0;i<256;i++) buf[i] = i;
	fp = fopen( "fuck.txt" , "r+" );

	fseek( fp , 512 , SEEK_SET );

	fwrite( buf , 1 , sizeof( buf ) , fp );

	fclose( fp );

}
