#include <time.h>

int main()
{
    char out[100];
    time_t t = time(NULL);
    struct tm *lt = localtime( &t );
    
    strftime( out, sizeof(out), "%Y-%m-%d-%H:%M", lt );

    printf( "%s\n",out);
}
