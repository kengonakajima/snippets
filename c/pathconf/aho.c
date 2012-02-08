#include <unistd.h>





int
main()
{
    printf( "LINK_MAX: %d\n", pathconf( "./aho.c" , _PC_LINK_MAX ) );
    printf( "MAX_CANON: %d\n", pathconf( "./aho.c" , _PC_MAX_CANON ));
    printf( "MAX_INPUT: %d\n", pathconf( "./aho.c" , _PC_MAX_INPUT));
    printf( "NAME_MAX: %d\n", pathconf( "./aho.c" , _PC_NAME_MAX ));
}
