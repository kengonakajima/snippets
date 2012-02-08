#include <stdio.h>

int pch( char *buf)
{
	int i;
	int cs=0,c;
	int state = 0;		/* 0 ... initial 1 .. \n\n after found  */
	int blanklinepos=-1;
	int len;

	len = strlen( buf );

	for(i = 0 ; i < len ; i++ ){

		c = buf[i];

		if( c == '\n' ){
			if( blanklinepos != -1 && blanklinepos < i-3 ) return 1;
		}

		if( c == '\n' && cs == '\n' ){
			state = 1;
			blanklinepos = i;
		}

		cs = c;
	}
	return 0;
}


main()
{
	char po[333] = "POST / HTTP/1.0\n\nasdfsadfasdfasdf";
	char tmp[243];

	printf("%d\n" , pch(po ));
	
}

