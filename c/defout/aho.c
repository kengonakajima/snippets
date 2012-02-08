
#include <stdio.h>

#define NO_OUT 1


#if NO_OUT == 1
#define ERROUT0(fmt)
#else
#define ERROUT0(fmt) vce_errout((fmt))
#endif

void vce_errout( char *hoge )
{
    fprintf( stderr, "%s aho \n", hoge );
}

int main()
{
    ERROUT0( "aho\n");
}
