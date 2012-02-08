#include <time.h>
#include <sys/time.h>

int
main()
{
    time_t t = time(NULL);
    struct tm* g;
    char *s;

    g = gmtime( &t);
    s = asctime(g);

    printf( "%s\n", s );
}
