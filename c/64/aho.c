#include <stdlib.h>

int main()
{
    long long a;
    long long b = (long long)65536 * 65536 * 65536;

    a = atoll( "111111111111" );

    printf( "%lld\n", a );
    printf( "%lld\n", b );    
//    printf( "%x\n", ((char*)(&b))[7]  & 0xff );    
    
}
