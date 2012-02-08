#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


main(int argc , char **argv )
{
	char hostname[109];
	
	struct hostent *hostptr;
	struct in_addr *ptr;

	gethostname( hostname , sizeof( hostname ));
	
	
	hostptr = gethostbyname( hostname );

	ptr = (struct in_addr * )(* hostptr->h_addr_list);
	
	printf("%s\n" , inet_ntoa( *ptr++ ));
	printf("%s\n" , inet_ntoa( *ptr++ ));
	printf("%s\n" , inet_ntoa( *ptr++ ));
	printf("%s\n" , inet_ntoa( *ptr++ ));
}
