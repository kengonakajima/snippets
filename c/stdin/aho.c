#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>


#define _MAIN_C_



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



double time_keeper_1=0.0;		/* loop_speedを有効なものにする */
double time_keeper_2=0.0;
double time_keeper_3=0.0;
double time_keeper_4=0.0;





main()
{

	char buf[10];
	
	fcntl( 0 , F_SETFL , FNDELAY );

	perror("aho");

}
