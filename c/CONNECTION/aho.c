/*
	Lifestorm$B$N(BCONNECTION$B7?$r%F%9%H(B   


*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<string.h>

#define		PORT	(u_short)80
#define		BUF_LEN	1000

char hostname[] = "lifestorm";
char buf[BUF_LEN];
char charlist[] = "CHARLIST type=login start=1 mode=visual\n";

//	select $B$9$k;~$N%S%C%H%^%9%/$N=`HwMQ(B
fd_set mask;
fd_set readok;

void main( void ){
	struct hostent *servhost;
	struct sockaddr_in server;
	struct timeval timeout;

	int s;
	int n;
	char c;

	timeout.tv_sec = 0 ;
	timeout.tv_usec = 0 ;

	//	$BAj<j$N%"%I%l%9$r(B server $B$H$$$&9=B$BN$K%;%C%H$9$k(B
	servhost = gethostbyname( hostname );
	bzero( (char *)&server, sizeof( server ) );
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	bcopy( servhost->h_addr, (char *)&server.sin_addr, servhost->h_length );

	//	$B%=%1%C%H$r3MF@$9$k(B
	s = socket( AF_INET, SOCK_STREAM, 0 );

	//	$BAj<j$N%"%I%l%9$r;XDj$7$F@\B3$NMW5a(B
	connect( s, &server, sizeof( server ) );

	//	$B%=%1%C%H%G%#%9%/%W%j%?$N%^%9%/(B
	FD_ZERO( &mask );
	FD_SET( 0, &mask );
	FD_SET( s, &mask );

	
/*	write( s, "adm\n", sizeof( "adm\n" ) ); */
	write( s, "NeWbu!1D\n", sizeof( "NeWbu!1D\n" ) ); 

	//	$B%a%$%s%k!<%W(B
	while( 1 ){

		//	$B%=%1%C%H$rD4$Y$k(B
		//	select $B$NA0$KKh2s(B readok $B$r=i4|2=$9$k(B 
		readok = mask;
		select( s+1, &readok, NULL, NULL, &timeout );

		//	$B%=%1%C%H$K%G!<%?$OMh$F$$$k$+(B
		if( FD_ISSET( s, &readok ) != 0 ){

			int	ret;
			ret = read( s , &buf , sizeof( buf ) );
			write( 1  , buf , ret );

		}
		//	$BI8=`F~NO$K%G!<%?$OMh$F$$$k$+(B
		if( FD_ISSET( 0, &readok ) ){
			write( s, "QUIT\n", sizeof( "QUIT\n" ) );		
			break;

		}

		sleep( 2 );
		write( s, charlist, strlen(charlist) );
	}		
	close( s );
}


/********************
  ASCII$BJ8;z$N(BIP$B%"%I%l%9$H%]!<%H$r;XDj$7$F!"%5!<%P!<$H$N@\B3$r(B
  $BF@$k!#(B

  char *hostname ... $B%[%9%HL>(B( ASCII )
  int port ... $B%]!<%HHV9f(B 
  
  return vlaues:
  	$B<:GT$9$k$H!"Ii$NCM$,JV$k$>@.8y$9$k$H(Bsockfd
	-1 ... gethostbyname error
	-2 ... socket error
	-3 ... connect error
	
********************/
int getServerSocket( char *hostname , int port )
{
	int sockfd ;
	char emsg[1000];
	struct hostent *servhost;
	struct sockaddr_in server;

	servhost = gethostbyname( hostname );
	if( servhost == NULL ){
		sprintf( emsg , "getServerSocket: gethostbyname() failed. host:[%s] errno:[%d]\n" , hostname  , errno );
		return -1;
	}
	bzero( (char*) &server ,sizeof( server ) );
	server.sin_family = AF_INET;
	server.sin_port = htons( port );
	bcopy( servhost->h_addr , (char *)&server.sin_addr , servhost->h_length );
	sockfd = socket( AF_INET , SOCK_STREAM , 0 );
	if( sockfd < 0 ){
		sprintf( emsg , "getServerSocket: socket() failed. errno:[%d]\n" , errno );
		return -2;
	}
	if( connect( sockfd  , &server , sizeof( server ) ) < 0 ){
		sprintf( emsg ,"getServerSocket: connect() failed. errno:[%d] hostname:[%s]\n" , errno , hostname );
		return -3;
	}

	return sockfd;
	
}


/********************
  CONNECTION$B7?$N%3%s%9%H%i%/%?4X?t!#(B

  arg[4] ... STRING IP$B%"%I%l%9(B(lifestorm.titan.co.jp/202.228.85.17$BN>J}<u$1$D$1$k$h$&$K$7$?$$(BPENDING)
  arg[3] ... NUMBER $B%]!<%HHV9f(B
  arg[2] ... $B%=%1%C%H%*%W%7%g%s(B
  arg[1] ... reserved
  arg[0] ... reserved
********************/
int sys_CONNECTION( char **arg , char **ret )
{
	int sockfd;

	sockfd = getServerSocket( arg[4] , getNumberFromNUMBERType( arg[3] ) );
	if( sockfd < 0 ){
		sprintf( ret[0] , "error ( getServerSocket returns:%d errno:%d )" , sockfd , errno );
		return -1;
	}

#if 0
	constructCONNECTIONType(
#endif

	
	return 0;
}



int main( int argc ,char **argv )
{

	
}

