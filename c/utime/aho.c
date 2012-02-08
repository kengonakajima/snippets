#include <sys/types.h>
#include <utime.h>

int main()
{
    struct utimbuf buf;
    int r;
    
    buf.actime = 1234599;
    buf.modtime = 123466990;
    
    r = utime( "hoge.txt",  &buf );
    printf( "%d\n", r );
        
}
