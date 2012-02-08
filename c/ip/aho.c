#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

main()
{
	unsigned int aho;
	struct sockaddr_in s;
	
	aho =  inet_addr( "192.168.1.11");

	s.sin_addr.s_addr = aho;
	s.sin_port = 808;
	
	printf( "%u\n" , aho );

	printf( "%s\n" , inet_ntoa(  s.sin_addr ) );
	printf( "%d\n", s.sin_port );
	
}
