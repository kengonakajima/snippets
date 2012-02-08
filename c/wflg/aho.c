#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


#include <sys/file.h>
#include <listen.h>

#include <sys/sockio.h>
#include <sys/socketvar.h>

#include <arpa/nameser.h>
#include <resolv.h>



int main( int argc , char **argv )
{


	int i;
	fd_set fdv,rfdv,wfdv;
	struct timeval tmsel;


	while(1)
	{
		tmsel.tv_sec = 0;
		tmsel.tv_usec = loop_speed;

		FD_ZERO( &fdv );
		FD_ZERO( &rfdv );
		FD_ZERO( &wfdv );




	}
	
}

