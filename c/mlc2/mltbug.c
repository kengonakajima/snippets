/*
 * mltbug.c
 *
 */
#include "mlc.h"
#include <gmp.h>
#include <stdio.h>



int main()
{
    mpz_t a,b,c,res;
    int bb;

    // hexadecimal or decimal?
    bb = 10; 

	mpz_init_set_str( a, "75093", bb );
	printf( "a = %s\n", mpz_get_str( NULL, bb, a ) );

	mpz_init_set_str( b, "8888", bb );
	printf( "b = %s\n", mpz_get_str( NULL, bb, b ) );

	mpz_init_set_str( c, "9", bb );
	printf( "c = %s\n", mpz_get_str( NULL, bb, c ) );

	mpz_powm( res, a,b,c );
	printf( "a^b mod c = %s\n", mpz_get_str( NULL, bb, res ) );
	
	return 1;

}

