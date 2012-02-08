
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{


	struct sockaddr_in a;
	unsigned long fuck;
	
	bzero( &a ,sizeof( a ) );

	a.sin_addr.s_addr = inet_addr( "192.168.1.51" );

	printf ( "%x\n" , a.sin_addr.s_addr );

	fuck = inet_netof(  a.sin_addr);

	printf ( "%x\n" ,fuck );
	
}
