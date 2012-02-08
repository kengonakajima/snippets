/*

  Diffie Hellman �����θ��򴹥롼����Υƥ���


  1. �����Ф� DHgenerator, DHmodulus , DHpubkey, DHprivate �����������ʳ�

  * generator �ϥǥե���Ȥ�"2"�Ǥ��롣�����˥Х��ʥ�����Ǥ�ꤿ���Τǡ�
  * modulus ��200ʸ�����餤��ʸ����Ǥ���(zbd)��
  DHpubkey 


  2. ���饤����Ȥ���DHgenerator,DHmodulus,DHpubkey���Ȥ�
     cliDHprivate, cliDHpub, shkey��    ���������ʳ�(cliDHprivate�ϡ�
     ��ξ��󤤤餺����Ω�ˤ�Ȥޤ�)

  3. �����Ф�shkey���Ѱդ����ʳ� ( cliDHpub, DHmodulus, DHprivate
     ���ɤߤ���Τ�)


     gcc aho.c -lgmp
     
 */

#include <stdio.h>
#include <gmp.h>

/*
  gmp��Ĥ��ä� Diffie Hellman ˡ���������롼����
  
 */
static void
diffie_hellman( char *genstr, char *modstr, char *expstr ,
                char *out, int outlen )
{
    mpz_t gen, mod , exp , key;
    char *keystr;

    mpz_init_set_str( gen , genstr , 16 );
    mpz_init_set_str( mod , modstr , 16 );
    mpz_init_set_str( exp , expstr , 16 );
    mpz_init( key );

    mpz_powm( key, gen, exp, mod );
    keystr = mpz_get_str( NULL , 16 , key );
    snprintf( out, outlen , "%s" , keystr );

    mpz_clear( gen );
    mpz_clear( exp );
    mpz_clear( mod );
    mpz_clear( key );
}

int
main( int argc, char **argv )
{

    /* step 1 */
    char generator[] = "2";
    char modulus[] =
        "f488fd584e49dbcd20b49de49107366b336c380d451d0f7c88b31c7c5b2d8ef6" \
        "f3c923c043f0a55b188d8ebb558cb85d38d334fd7c175743a31d186cde33212c" \
        "b52aff3ce1b1294018118d7c84a70a72d686c40319c807297aca950cd9969fab" \
        "d00a509b0246d3083d66a45d419f9c7cbd894b221926baaba25ec355e92f78c7";
    char privatekey[] = "9123456701234567012345670123456701234567";
    char DHpubkey[1000];
    char cliprivatekey[] = "1111111122222222333333334444444477777777";
    char cliDHpubkey[1000];
    char secretkey[1000] , clisecretkey[1000];

    diffie_hellman( generator , modulus , privatekey,
                    DHpubkey, sizeof( DHpubkey ));

    printf( "DHpubkey:\n%s\n\n", DHpubkey ); 

    /* step 2 */
    diffie_hellman( generator, modulus, cliprivatekey,
                    cliDHpubkey, sizeof( cliDHpubkey ) );

    printf( "cliDHpubkey:\n%s\n\n", cliDHpubkey );
    diffie_hellman( DHpubkey, modulus, cliprivatekey,
                    clisecretkey , sizeof( clisecretkey ) );
    printf( "clisecretkey:\n%s\n\n", clisecretkey );


    /* step 3 */
    diffie_hellman( cliDHpubkey, modulus, privatekey,
                    secretkey, sizeof( secretkey ) );
    printf( "secretkey:\n%s\n\n", secretkey );
    
}
