#include <stdio.h>

int main()
{

	FILE *fp;

	fp = fopen("hoge", "w+" );
	fprintf( fp , "asdkfjhaskdfjhasldf\n" );
	fclose(fp);

}
