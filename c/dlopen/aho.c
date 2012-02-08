#include <dlfcn.h>

int
main( int argc, char **argv )
{
    void *h1;
    void *h2;

    h1 = dlopen( "libmmm.so" , RTLD_LAZY | RTLD_GLOBAL );
    printf( "h1: %p\n", h1 );

    setenv( "LD_LIBRARY" , "/home/ringo/test/dlopen/aho" , 1 );

    h2 = dlopen( "libmmm.so" , RTLD_LAZY | RTLD_GLOBAL );
    printf( "h2: %p\n", h2 );

}
