#include <stdio.h>


#define RAND() ( random() % 32768 )











main(int argc , char **argv )
{
	int i;
	
	for(i=0;i<100;i++){
		printf( "%d\n"  , getNumberFromRandomString( argv[1] ) );
	}	
	
	
}




int countCharEscaped( char *buf , int escc , int c ,int n)
{
    int i;
    int count = 0;
    for(i = 0 ; ;i++){
		if( buf[i] == '\0' ) break;
		if( buf[i] == c && i > 0 && buf[i-1] != escc ){
			count++;
			if( count == n ) return i;
		}
    }

    return -1;
}


int countChar( char *buf , int c ,int n)
{
    int i;
    int count = 0;
    for(i = 0 ; ;i++){
		if( buf[i] == '\0' ) break;
		if( buf[i] == c ){
			count++;
			if( count == n ) return i;
		}
    }

    return -1;
}


int randMinMax( int imin , int imax )
{
	int tmp;
	if( imin > imax){
		tmp = imax;
		imax = imin;
		imin = tmp;
	}

	if( (imax-imin)<=0) return imin;

	tmp = imin + ( RAND() % (imax - imin) );
	return tmp;
	
}






	 
int getNumberFromRandomString( char *rstr )
{

	int i1,i2;

	i1 = countChar( rstr , '*' , 1 );
	i2 = countChar( rstr , '*' , 2 );

	if( i1 >= 0 && i2 >= 0){
		/*  '*'$B$,(B2$B8D$"$C$?!#$3$l$O6Q0lJ,I[$G$"$k!#(B */
		int v;
		v =  randMinMax( atoi( rstr + i1 +1 ) , atoi( rstr + i2 +1) );
		
		return v;
		
	} else if( i1 >= 0 && i2 < 0 ){
		/*  '*'$B$,(B1$B8D$"$C$?!#$3$l$O3NN($K$h$k@53N$JCM$G$"$k!#(B */
		int sikii,val;
		int c;
		c = countChar( rstr , 'R' , 1 );
		if( c >= 0 ){
			sikii = atoi( rstr + c +1);
		} else {
			c = countChar( rstr , '%' , 1 );
			if( c >= 0 ){
				sikii = 328 * atoi( rstr + c +1);
			} else {

				return 0;
			}
		}

		c = countChar( rstr , '*' , 1 );
		val = atoi( rstr  + c +1);

		if( (c = RAND()) < sikii ){
			return val;
		} else {
			return 0;
		}
	} else {
		if( countChar( rstr , '%' , 1 ) < 0 && countChar( rstr , 'R' , 1 ) < 0 ){
			/* $B5-9f$,4^$^$l$F$$$J$+$C$?$N$G!"DL>o$N?tCM$H$7$FJV$9!#(B */

			return atoi( rstr );
		}
	}

	printf( "fuck\n" );
	return 0;
}


