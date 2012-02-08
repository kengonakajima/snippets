#include <time.h>
#include <stdio.h>

int main()
{
        time_t t = 19389;
        char *cc = ctime( &t );
        printf("%s\n", cc );
}
