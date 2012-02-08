/*
 * mltcomp.c
 *     by Thai 00/07/15
 */
#include "mlc.h"
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/* slint --- print out LINT */
void slint( LINT la ){
	int a;
	printf( "Len=%d:\t", la.len );
//	for( a = la.len; a > 0; a-- ){
	for( a = la.len<5?5:la.len; a > 0; a-- ){
		printf( "%4d ", la.num[a] );
	}
	printf( "\n" );

}

void create_random( int digits, char* line, int base ){
    int i, t;
    if( base == 16 ){
        t = rand() % 15 + 1;
        line[0] = t<10 ? t+'0' : t-10+'a';
        for( i=1; i<digits; i++){
            t = rand() % 16;
            line[i] = t<10 ? t+'0' : t-10+'a';
        }
    } else {
        line[0] = ( rand() % 9 ) + 1 + '0';
        for( i=1; i<digits; i++){
            line[i] = ( rand() % 10 ) + '0';
        }
    }
    line[i] = '\0';
}

int main()
{
	char in[MAXLEN*4];
	char out[MAXLEN*4];
	LINT la, lb, ans, ln;
	int r;
    mpz_t a,b,c,res;
    int count;
    int diga, digb, digc;
    int bb;
    char line[1000];
    char ca[MAXLEN*4],cb[MAXLEN*4],cc[MAXLEN*4]; //debugging

    // hexadecimal or decimal?
    bb = 10; 

    srand( (int)time(NULL) );
    count = 0;
    
	for(;;){


        // how many digits should the random numbers be?
        diga = rand()%80+2;
        digb = diga-1;
        digc = rand()%30+1;

        fprintf( stderr, "%d\r", count++ ); 
        create_random(diga,line,bb);
        strncpy( ca, line, sizeof(line) ); // just for debugging
		r = ml_str2LINT( line, sizeof( line ), &la, bb );
		if( r < 0 ) printf( "a:parse error a\n" );
        mpz_init_set_str( a, line, bb );
        
		
        create_random(digb,line,bb);
        strncpy( cb, line, sizeof(line) );// just for debugging
		r = ml_str2LINT( line, sizeof( line ),  &lb,bb );
		if( r < 0 ) printf( "b: parse error\n" );
        mpz_init_set_str( b, line, bb );

        create_random(digc,line,bb);
        strncpy( cc, line, sizeof(line) );// just for debugging
        //        strncpy( line, "6561", sizeof( "6561" ) );
		r = ml_str2LINT( line, sizeof( line ),  &ln, bb );
		if( r < 0 ) printf( "c,n: parse error\n" );
        mpz_init_set_str( c, line, bb );
        

        mpz_init( res );

        r = ml_add( la, lb, &ans );
		if( r < 0 ) printf( "add error!\n" );
		r = ml_LINT2Str( out, ans );
		if( r < 0 ) printf( "add LINT2Str error!\n" );       
        //		printf( "a+b=\n%s\n", out);
//		slint( ans );
        mpz_add( res, a,b );
        //		printf( "a+b=\n%s\n", mpz_get_str( NULL, 10, res ) );
        if( strcmp( out, mpz_get_str( NULL, 10, res ) ) != 0 ){
            printf( "no match:\n" );
            slint( la );
            slint( lb );
            slint( ans  );
            getchar();
        }
        
        
		
		r = ml_sub( la, lb, &ans );
		if( r < 0 ) {
            printf( "sub error!\n" );
            printf( "%s\n", ca );
            slint( la );
            printf( "%s\n", cb );
            slint( lb );
        }
		r = ml_LINT2Str( out, ans );
		if( r < 0 ) {
            printf( "sub lint2str error!\n" );
            slint( la );
            slint( lb );
            slint( ans  );
        }
        //		printf( "a-b=\n%s\n", out);
//		slint( ans );
        mpz_sub( res, a,b );
        /*        
        if( strcmp( out, mpz_get_str( NULL, 10, res ) ) != 0 ){
            printf( "sub no match:\n" );
            slint( la );
            slint( lb );
            slint( ans  );
            getchar();
        }
        */
        //		printf( "a-b=\n%s\n", mpz_get_str( NULL, 10, res ) );
		
		r = ml_mul( la, lb, &ans );
		if( r < 0 ) printf( "mul error!\n" );
		r = ml_LINT2Str( out, ans );
		if( r < 0 ) printf( "mul lint2str error!\n" );
        //		printf( "a*b=\n%s\n", out);
//		slint( ans );
        mpz_mul( res, a,b );
        if( strcmp( out, mpz_get_str( NULL, 10, res ) ) != 0 ){
            printf( "no match:\n" );
            slint( la );
            slint( lb );
            slint( ans  );
            getchar();
        }
        //		printf("a*b=\n%s\n", mpz_get_str( NULL, 10, res ) );

		r = ml_powm( la, lb, ln, &ans );
		if( r < 0 ) printf( "powm error!\n" );
		r = ml_LINT2Str( out, ans );
		if( r < 0 ) {
            printf( "powm lint2str error!\n" );
            slint( ans );
        }
        //		printf( "la^lb mod ln =\n%s\n", out);
		//slint( ans );
        mpz_powm( res, a,b,c );
        if( strcmp( out, mpz_get_str( NULL, 10, res ) ) != 0 ){
            printf( "[%d] Powm: no match.\n", count );
            slint( la );
            slint( lb );
            printf( "a: %s\n", ca );
            printf( "b: %s\n", cb );
            printf( "c,n: %s\n", cc );
            slint( ln );
            printf( "ml_powm: %s\n", out );
            slint( ans );
            printf( "mpz_powm: %s\n", mpz_get_str( NULL, 10, res ) );
            getchar();
        }
        //		printf( "la^lb mod ln =\n%s\n", mpz_get_str( NULL, 10, res ) );
		
        //		getchar();
	}	
		return 1;

}

