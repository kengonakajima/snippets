#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main()
{
    struct sockaddr_in aa;
    
    bind( 10 , (struct sockaddr * ) &aa , sizeof( struct sockaddr_in ));
    // bind( 10 ,  &aa , sizeof( struct sockaddr_in ));
    
    return 0;
}
