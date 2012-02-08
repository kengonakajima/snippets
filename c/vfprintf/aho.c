#include <stdio.h>
#include <stdarg.h>

void
hoge( int d, char *format, ... )
{
    va_list ap;
    va_start( ap,format );
    vfprintf( stderr, format, ap );
    vfprintf( stderr, format, ap );    
    va_end(ap);
}

int
main()
{
    hoge( 10, "%d %s\n", 10, "hoge" );
    hoge( 10, "%d %s\n", 20, "noge" );
    hoge( 10, "%d %s\n", 30, "moge" );    
}

    
