#include <stdio.h>

double  linearDiv( double val1 , double val2 , double d );

int main()
{
    int aho = (int)linearDiv( 100, 200 , 0.8 );
    printf( "aho: %d\n" , aho );
}
/*
 * 線型で内分点をもとめる。
 *
 *
 * double val1 , val2 : この値のあいだをとる
 * double d : 内分率
 *
 *
 *  -d<0----- val1 ---0<d<1------- val2 ------d>1----
 *
 *
 */
double  linearDiv( double val1 , double val2 , double d )
{
    return val1 + ( val2 - val1 ) * ( d );
}
