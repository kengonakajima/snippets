#include <iostream>

#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>


int main()
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo( "localhost", "11000", &hints, &res );
    int new_fd = socket( res->ai_family, res->ai_socktype, res->ai_protocol);
    if( new_fd == -1 ){
        std::cerr << "socket error" << std::endl;
        return 0;
    }

    if( connect( new_fd, res->ai_addr, res->ai_addrlen ) == -1 ){
        close(new_fd); // connect error
        return 0;
    }

    //shutdown( new_fd, SHUT_WR );

    // shutdown SHUT_WRすると、クライアントはFIN_WAIT_2になり、サーバはCLOSE_WAITになる. close(fd)も同じ。
    // SHUT_RDにすると状態変化はない。

    //    close( new_fd );
    
    while(1){
        usleep(1000);
    }
    return 0;
}
