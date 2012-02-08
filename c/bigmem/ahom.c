#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

/*
	$BBg$-$$%a%b%j$r07$&%F%9%H(B

	400MByte$BDxEY$N%a%b%j$,$"$C$F!"$[$H$s$I$N>l=j$,%"%/%;%9$5$l$F$$$J$$(B
	$B$,!"ItJ,E*$K$O$+$J$jIQHK$K%"%/%;%9$5$l$F$$$k$h$&$J>l9g!"%9%o%C%W(B
	$B$NNN0h$O$I$N$h$&$K$J$j!"%a%b%j;HMQNL$O$I$N$h$&$K$J$k$N$+$r$7$i$Y$k!#(B

	*-----*-----*-----*-----*-----*-----*-----*-----*-----*


	-*-----*-----*-----*-----*-----*-----*-----*-----*-----
	--*-----*-----*-----*-----*-----*-----*-----*-----*----
	---*-----*-----*-----*-----*-----*-----*-----*-----*---
	----*-----*-----*-----*-----*-----*-----*-----*-----*--
	-----*-----*-----*-----*-----*-----*-----*-----*-----*-

	$B$H$$$&$h$&$K%"%/%;%9$7$F$_$k!#(B
*/   

#define BSIZ	(50*1024*1024)

#define INTERVAL 4096*512		/* $B%P%$%H$4$H$K(B */
#define AMOUNT	 1			/* $B%P%$%H$NO"B3NN0h$r%"%/%;%9(B */

double getUTimeDouble(void);

int main(int argc ,char **argv )
{
	int i,j ;
	int c;
	int start =0;
	char *mem;
	double t;
	int bsiz = BSIZ;
	
	mem = (char *)malloc( BSIZ );
	if( mem == NULL ) {
		fprintf( stderr,"fuck!");
		exit(0);
	}

	/* print estimated time */

	c = start;
	for(i=0;;i++){
		t = getUTimeDouble();
		for(j=0;j<AMOUNT;j++){
			mem[c+j] = j;
			/*int mmmm = mem[c+j];*/
			
		}

		fprintf( stderr,"Time:%f c:%x\n" , getUTimeDouble() - t , c );
		c += INTERVAL;
		if( c >= (BSIZ-AMOUNT)) break;
	}
	
}

double getUTimeDouble(void)
{

	struct timeval tm;

	gettimeofday( &tm , NULL );


    return (double)( tm.tv_usec ) + (double)( tm.tv_sec) * 1000000.0 ;
}

