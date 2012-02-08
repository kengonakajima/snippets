
#include <unistd.h>

int
main()
{

    int a;
    printf( "SC_ARG_MAX %d\n", sysconf( _SC_ARG_MAX ) );
    printf( "SC_CHILD_MAX %d\n", sysconf( _SC_CHILD_MAX ) );
    printf( "SC_STREAM_MAX %d\n", sysconf( _SC_STREAM_MAX ) );
    printf( "SC_TZNAME_MAX %d\n", sysconf( _SC_TZNAME_MAX ) );
    printf( "SC_OPEN_MAX %d\n", sysconf( _SC_OPEN_MAX ) );
    printf( "SC_JOB_CONTROL %d\n", sysconf( _SC_JOB_CONTROL ) );
    printf( "SC_SAVED_IDS %d\n", sysconf( _SC_SAVED_IDS ));
    printf( "SC_VERSION %d\n", sysconf( _SC_VERSION ) );

}
