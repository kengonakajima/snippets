#include <stdio.h>


main(int argc , char **argv)
{

	int i;

	for(i = 0 ; i < 256 ; i++ ){
		char buf[10], buf2[10];
		buf[0] = 'r';
		buf[1] = '\0';
		buf2[0] = i;
		buf2[1] = '\0';
		printf("%c %d\n" , i,strcmp( buf , buf2 ) ) ;
	}
	
	
}
