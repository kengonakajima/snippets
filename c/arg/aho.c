
#include <stdio.h>
#include <stdarg.h>


#if defined WIN32
#define _outputDebugString outputDebugString
#define __vsnprintf _vsnprintf
#else
#define _outputDebugString(x) printf("%s",x)
#define __vsnprintf vsnprintf
#endif

#define PRINTE qqq( __FILE__, __LINE__ ) + printe

int
qqq( char *f, int l )
{
    char hoge[2000];
    snprintf( hoge, sizeof(hoge), "%s:%d: ", f,l );
    _outputDebugString( hoge );
    return 0;
}

int
printe( char *format, ... )
{
    char *p;
    int l;
    va_list va;

    l = 1000;
    p = (char*)malloc(l);

    va_start(va,format);
    
    for(;;){
        __vsnprintf( p, l, format, va );
        if( strlen(p) == (l-1) ){
            l *= 2;
            p = (char*) realloc( p, l );
        } else {
            _outputDebugString(p);
            free(p);
            va_end(va);
            break;
        }
    }
    return 0;
}

int main()
{
    int i;
    for(i=0;i<1000;i++){
        PRINTE( "%d %s\n",10, "askdhhfldhfodhflkajhdfsdlfjhsadf"            );
    }
    malloc_stats();
}

