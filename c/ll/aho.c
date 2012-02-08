unsigned long long a;

#include <stdio.h>


int main()
{
    a = (unsigned long long) 65536* 65536*65536*12345;
    printf( "%lld\n", a);
}
