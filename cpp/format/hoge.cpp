#include <stdio.h>
#include <stdarg.h>

class Format {
public:
    char buf[1024];
    Format( const char *fmt, ... ) {
        va_list argptr;
        va_start( argptr, fmt );
        vsnprintf( buf, sizeof(buf), fmt, argptr );
        va_end( argptr );
    }
};

int main() {
    for(int i=0;i<100000000;i++) {
        printf("%s\n", Format( "%d %s", 5, "ringo" ).buf );
    }
    
}

