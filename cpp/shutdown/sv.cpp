#include <iostream>

#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>

#if 0
int pdn::makeClientSocket( const char *addr, const char *port )
{
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo( addr, port, &hints, &res );
    int new_fd = socket( res->ai_family, res->ai_socktype, res->ai_protocol);
    if( new_fd == -1 ){
        std::cerr << "socket error" << std::endl;
        return 0;
    }

    if( connect( new_fd, res->ai_addr, res->ai_addrlen ) == -1 ){
        close(new_fd); // connect error
        return 0;
    }
    
#ifdef WIN32
    DWORD  dwNonBlocking = 1;
    ioctlsocket(new_fd, FIONBIO, &dwNonBlocking);
#endif

    freeaddrinfo(res);
    return new_fd;    
}


int pdn::makeServerSocket( const char *addr, const char *port )
{
    struct addrinfo hints, *res;
    memset(&hints,0,sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(addr, port, &hints, &res );
    int fd = socket( res->ai_family, res->ai_socktype, res->ai_protocol );

    int opt = 1;
    if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) == -1 ){
      std::cerr << "setsockopt(reuseaddr) error:" << strerror(errno) << std::endl;
      close(fd); // setsockopt error
      return 0;
    }
    
    if( bind( fd, res->ai_addr, res->ai_addrlen ) == -1 ){
        std::cerr << "bind error" << std::endl;
        close(fd); // bind error
        return 0;
    }
    if( listen( fd, SOMAXCONN ) == -1 ){
        std::cerr << "listen error" << std::endl;
        close(fd); // listen error
        return 0;
    }

#ifdef WIN32
    DWORD  dwNonBlocking = 1;
    ioctlsocket(fd, FIONBIO, &dwNonBlocking);
#endif

    freeaddrinfo(res);

    return fd;
}

#endif


int main()
{
    struct addrinfo hints, *res;
    memset(&hints,0,sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo( "localhost", "11000", &hints, &res );
    int fd = socket( res->ai_family, res->ai_socktype, res->ai_protocol );

    int opt = 1;
    if( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) == -1 ){
      std::cerr << "setsockopt(reuseaddr) error:" << strerror(errno) << std::endl;
      close(fd); // setsockopt error
      return 0;
    }
    
    if( bind( fd, res->ai_addr, res->ai_addrlen ) == -1 ){
        std::cerr << "bind error" << std::endl;
        close(fd); // bind error
        return 0;
    }
    if( listen( fd, SOMAXCONN ) == -1 ){
        std::cerr << "listen error" << std::endl;
        close(fd); // listen error
        return 0;
    }


    int new_fd;
    struct sockaddr nsa;
    socklen_t nsalen = sizeof(nsa);
    new_fd = accept( fd, &nsa, &nsalen);

    //    shutdown( new_fd, SHUT_WR );
    close( new_fd);
    // SHUT_RDしても何も起きない
    // SHUT_WRすると active close開始する . closeでも同様。
    
    while(1){        
        usleep(1000);
    }
    return 0;

}
