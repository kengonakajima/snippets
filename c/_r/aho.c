#include <time.h>

int main()
{
    time_t t;
    
    localtime(  &t );

    localtime_r( &t );

    
}
