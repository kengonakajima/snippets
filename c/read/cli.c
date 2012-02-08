#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

/*
   $B3+H/Cf$K$O%/%i%$%"%s%H$,$J$$$?$a!"<B83MQ$N$b$N!#(B
   curses$B$r;H$&!#(B
   $BCO7A$NI=<($O!"$"$k!"$J$7E*$JC1=c$J$b$N$K$9$k!#(B
   
*/

/*
   $B$3$l$O!"%/%i%$%"%s%H$,;}$D$Y$-%G!<%?$J$N$G!"%5!<%P!<$H$O4X78$J$$!#(B
   java$B$N%3!<%G%#%s%0$N;29M$K$J$k$@$m$&!#(B

*/
/****************************************************************
	curses$B%P!<%8%g%s$NA`:nJ}K!(B


	hjkl  	4$BJ}8~0\F0(B ($B86B'$H$7$F!"(B4$BJ}8~$7$+F0$1$J$$(B )
	a	$B967b(B

	


	
****************************************************************/

#define IPADDRESS "192.168.1.11"
#define PORT 9000

int main(int argc , char **argv )
{
	int sockfd ;
	struct sockaddr_in serv_addr;
	int siz;
	
	if( argc== 1 ){
		fprintf(stderr,"usage: ctest siz \n");
		exit(1);
	}

	siz = atoi( argv[1] );
	
	   /* $B%=%1%C%H$r3+$/(B */
	
	bzero(( char *) &serv_addr , sizeof( serv_addr ));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr( IPADDRESS );
	serv_addr.sin_port = htons( PORT );

	if( ( sockfd = socket( AF_INET , SOCK_STREAM , 0 )) < 0 ){
	    	fprintf(stderr,"client:socket error.\n");
		exit(1);
	}

	if( connect( sockfd ,( struct sockaddr *) & serv_addr , sizeof( serv_addr ))< 0 ){
	    	fprintf(stderr , "client: connect error.\n");
		exit(1);
	}
	{
		int data[32768];
		int i;
		for(i = 0 ; i < siz ; i++ ) data[i] = 0;
		for(i = 0; ;i++ ){
			int n;
			sleep(1);
			data[0] = i;
			data[siz-1] = -i;
			n = write(sockfd , data , ( siz * 4 ) );
			fprintf(stderr, "client:wrote %dbytes.\n" , n);
				n = write(sockfd , data , ( siz * 4 ) );
			fprintf(stderr, "client:wrote %dbytes.\n" , n);
			
		}
	}

	
}
