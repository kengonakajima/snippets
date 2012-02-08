#include <stdio.h>

main(int argc  ,char **argv)
{
	if( argc <= 1 ) {
		return;
	}
	printf("%s\n" , crypt( argv[1] , "rr") );
}

