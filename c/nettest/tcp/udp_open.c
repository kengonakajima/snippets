/*
	$B!V(BUNIX $B%M%C%H%o!<%/%W%m%0%i%_%s%0(B $B!W$K:\$C$F$$$?!"(BUDP$B$r;H$&$?$a$N(B
	$B%f!<%F%#%j%F%#%W%m%0%i%`!#$3$l$G%=%1%C%H$r3+$/$3$H$,$G$-$k(B

*/

#include "netdefs.h"

#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef INADDR_NONE
#define INADDR_NONE	0xffffffff
#endif

/*
	$B<!$NBg0hJQ?t$O!"I,MW$G$"$l$P8F$S=P$7B&$G;2>H$9$k$3$H$,$G$-$k!#(B
*/
struct sockaddr_in	udp_srv_addr;	/* $B%5!<%P$N(BInternet$B%=%1%C%H%"%I%l%9(B */
struct sockaddr_in	udp_cli_addr;	/* $B%/%i%$%"%s%H$N(B $B!&!&!&(B */
struct servent		udp_serv_info;	/* getservbyname() $B$NLa$jCM(B */
struct hostent		udp_host_info;  /* gethostbyname() $B$NLa$jCM(B */



/*
   $B4X?t(B udp_open
   host ...  $B8r?.$9$kB>%7%9%F%`$NL>A0(B
   service ... $BMW5a$5$l$F$$$k%5!<%S%9$NL>A0(B,port>0$B$N;~$@$1!"(BNULL$B$G$b9=$o$J$$(B
   port ...  ==0$B$J$i!"(Bservice$B$GDj5A$5$l$F$$$k%]!<%H$rMQ$$$k(B
    	     <0 $B$J$i!"%m!<%+%k$JM=Ls:Q$_%]!<%H$r%P%$%s%I$9$k(B
	     >0 $B$J$i!"$=$NCM$r%5!<%P$N%]!<%HHV9f$H$7$FMQ$$$k(B( $B%[%9%H%P%$%H=g=x(B)
   dontconn ... 0$B$N$H$-$@$1!"(Bconnect$B$r8F$S$@$9!#(B

*/
int udp_open( char *host , char *service , int port , int dontconn )
{
	int fd ;
	unsigned long inaddr;
	char *host_err_str();
	struct servent *sp;
	struct hostent *hp;

	/* $B%5!<%P$N(BInternet$B%"%I%l%9$r=i4|2=$9$k!#%]!<%H$b4^$`!#(B */

	bzero(( char *) &udp_srv_addr , sizeof( udp_srv_addr ));
	udp_srv_addr.sin_family = AF_INET;

	if( service != NULL ){
		if( ( sp = getservbyname( service , "udp")) == NULL ){
			perror("udp_open:unknown service: %s/udp" , service );
			return (-1);
		}
		udp_serv_info = *sp;
		if( port > 0 )
			udp_srv_addr.sin_port = htons( port );
		else
			udp_srv_addr.sin_port = sp->s_port;
	} else {
		if( port <= 0 ){
			perror("udp_open: must specify either service or port " );
			return -1;
		}
		udp_srv_addr.sin_port = htons( port );
	}

	/* $B:G=i$O!"(B 192.168.54.48 $B$N$h$&$J7A<0$G$"$k$H$7$FJQ49$9$k!#<:GT$7$?$i!"L>A0$H$9$k(B */

	if( ( inaddr = inet_addr( host  )) != INADDR_NONE ){
		bcopy(( char * ) &inaddr , (char *) &udp_srv_addr.sin_addr , sizeof( inaddr ));
		udp_host_info.h_name = NULL;
	} else {
		if( ( hp = gethostbyname(host ) ) == NULL ){
			perror("udp_open:host name error: %s %s " , host , host_err_str());
			return -1;
		}
		udp_host_info = *hp;
		bcopy( hp->h_addr , (char * ) &udp_srv_addr.sin_addr , hp->h_length );
	}

	if( port < 0 ){
		perror("udp_open: reserved ports not implemented yet");
		exit(-1);
	}

	if( ( fd = socket( AF_INET , SOCK_DGRAM ,  0 ) ) < 0 ){
		perror("udp_open: can't create UDP socket.");
		return (-1);
	}

	/* $BE,Ev$J%m!<%+%k%"%I%l%9$r%P%$%s%I$9$k(B */

	bzero( ( char *) &udp_cli_addr , sizeof( udp_cli_addr ) );
	udp_cli_addr.sin_family 	= AF_INET;
	udp_cli_addr.sin_addr.s_addr 	= htonl(INADDR_ANY );
	udp_cli_addr.sin_port 		= htons( 0 );
	if( bind( fd , (struct sockaddr * ) &udp_cli_addr ,sizeof( udp_cli_addr )) < 0 ){
		perror("udp_open: bind error." );
		close(fd );
		return -1;
	}
	
	/* $BI,MW$J$i!"(Bconnect$B$r8F$V!#Aj<j$,JQ$o$k$3$H$O$J$$$N$G!"DL>o$O8F$V!#(B
	   $B$3$l$r8F$s$G$*$/$H!"8F$S=P$7B&$G$O!"(B send() $B$H(B recv $B$r;H$($k$h$&$K$J$k!#(B
	*/

	if( dontconn == 0 ){
		if( connect( fd , (struct sockaddr *) &udp_srv_addr , sizeof(udp_srv_addr ))< 0 ){
			perror(" udp_open: connect error.");
			return -1;
		}
	}
	return fd;
}

