/*
  gmp のてすと
  
 */

#include <gmp.h>
int
main( int argc, char **argv )
{
    mpz_t a;
    char hoge[1111];

    mpz_init_set_str( a, "0010", 16 );

    printf( "'%s'\n", mpz_get_str( 0, 10, a ) );

    mpz_clear( a );

    
}
