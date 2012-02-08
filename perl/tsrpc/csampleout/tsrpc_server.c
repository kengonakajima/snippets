#define _TSRPC_SERVER_C_


#include "tsrpc.h"



/****************
    $B%G%#%9%Q%C%A%k!<%A%s(B

	char * buffer : $B%P%C%U%!(B(readbuf )
	int sockfd;     $BBP>](Bfd 
	
	buffer$B$K$OIaDL$N(B

 buffer$B$rFI$s$G%a%b%j$r3NJ]$7$F4X?t$r8F$S$@$7$F(Bsockfd$B$G(BwriteLine$B$9$k(B

****************/
void tsrpcServerDispatcher( char *buffer , int sockfd )
{

	char **tk;

	tk = modifySplitString( buffer );
	
	if( strcmp( tk[0], "remotesum2" ) == 0 ){
		char *str = (char*)malloc( MKSTR_MINSIZE );
		int ret;
		int *intbuf;
		intbuf = (int*)malloc( sizeof( int ) * 10 );
		ret = remotesum2(
						demkstr_string( tk[1] ) ,
						demkstr_int32_array( tk , intbuf, 2 , 10 ) );

		str = mkstr_int32( str , ret );
		writeLine( sockfd , str );
		free( intbuf );
		free( str );

	} else {
		writeLine( sockfd , "server:undefind function." );
	}
}



/************
  $B%5!<%P!<$r:n$k(B

  $B$?$/$5$s%5!<%P!<$r$D$/$C$F%^%k%A%9%l%C%I$G$?$/$5$s%5!<%S%9$9$k$H$+$b2D(B
************/
SERVER* tsrpcCreateServer( char *local , int port ,
					void(*loopfunc)(void))
{
	struct sockaddr_in sin;

	SERVER *s = (SERVER*)malloc( sizeof( SERVER ) );
	if(s == NULL ){
		fprintf( stderr , "cannot malloc SERVER struct.\n" );
		return (SERVER*)NULL;
	}
	bzero( s , sizeof( SERVER ) );

	/* $B%=%1%C%H$r:n$C$F%P%$%s%I$7$F(Blisten$B$9$k(B */

	s->tcp_sockfd = socket( AF_INET , SOCK_STREAM , 0 );
	if( s->tcp_sockfd < 0 ){
		fprintf( stderr, "socket(tcp) failed.\n" );
		return NULL;
	}
	s->udp_sockfd = socket( AF_INET , SOCK_DGRAM , 0 );
	if( s->udp_sockfd < 0 ){
		fprintf( stderr , "socket(udp) failed.\n" );
		return NULL;
	}
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons( port );

	if( local == (char*)NULL ){
		sin.sin_addr.s_addr = INADDR_ANY;		
	} else {
		/* $B$J$^$((B $B$r$D$1$k$>(B */
		struct hostent *h = gethostbyname( local );
		if( h == NULL ){
			fprintf( stderr , "gethostbyname failed. errno: %d\n" , errno );
			return NULL;
		}
		bcopy( h->h_addr_list[0] , &sin.sin_addr , 4 );
	}
	s->loopfunc = loopfunc;
	listen( s->tcp_sockfd , 5 );

	return s;
}

/************
  $B%5!<%P!<$N%a%$%s%k!<%W(B

  char *local : $B%m!<%+%k%[%9%H$NL>A0$E$1$r$9$k$+$I$&$+!#(B NULL $B$r;XDj$9$k$H(B
  INADDR_ANY$B$K$J$k(B
  int port : port
  void (*loopfunc)(void) : $B%k!<%W$4$H$K8F$S$@$94X?t(B

  $BFI$_$3$_$b=q$-$3$_$b2DG=$J$+$.$j$d$C$F!"(B1$B9TJ,$=$m$C$F$$$k$H$3$m$r$_$D$1$k$H(B
  ($B$=$l$O$+$J$i$:$"$?$i$7$/FI$_$3$_$,$"$C$?$H$3$m$G$"$k$+$i9bB.2=$K$J$k$>(B)
  $B$=$N%=%1%C%H$K$D$$$F=hM}$r$9$k(B

************/
void tsrpcServerMainLoop( SERVER*sv )
{
	int i;

	while(1){
		fd_set rfds , wfds;


		FD_ZERO( &rfds );
		FD_ZERO( &wfds );
		FD_SET( sv->tcp_sockfd , &rfds );
		FD_SET( sv->udp_sockfd , &rfds );
		FD_SET( sv->tcp_sockfd , &wfds );
		FD_SET( sv->udp_sockfd , &wfds );
		for(i =0 ;i<MAXSOCKFD;i++){
			if( sv->sockuse[i] != 0 ){
				FD_SET( i , &rfds );
				FD_SET( i , &wfds );
			}
		}

		select( 30 , &rfds , &wfds, (fd_set *)NULL , 0 );

		/* $B%a%$%s%=%1%C%H$KF~NO$,$-$?$J$i(Baccept$B$9$k$N$@(B */
		if( FD_ISSET( sv->tcp_sockfd ,&rfds ) ){
			struct sockaddr_in c;
			int len;
			int newsockfd = accept(sv->tcp_sockfd , (struct sockaddr *) &c ,  &len );
			if( newsockfd > 0 ){
				sv->sockuse[newsockfd] = 1;       /* $B%U%i%0$@$1$G$h$$(B */
				sv->cli[newsockfd] = c;    
			}
		}

		/* $B$3$I$b%=%1%C%H$K$-$?$J$iFI$_$3$`$N$@(B */
		for(i=0;i<MAXSOCKFD;i++){
			if( sv->sockuse[i] && FD_ISSET( i ,&rfds ) ){
				char buf[READBUFSIZE];
				int readlen, newbuflen , linelen;
				readlen = read( i , buf , sizeof( buf  ) );
				if( readlen <= 0 ){
					/* $B%=%1%C%H$,$U$!$C$/$7%?(B */
					close( i );
					continue;
				}
				/* $B$A$c$s$HFI$_$3$a$?$+$i%P%U%!$K$/$o$($k(B */


				newbuflen = ( readlen + sv->readbuf_use[i] );
				/* $B%5%$%:$r1[$($?$i:F3NJ](B */
				if( newbuflen > sv->readbufsize[i] ){     				
					/* $B5l%P%C%U%!$,$J$$>l9g$O3NJ](B */
					if( sv->readbuf[i] = (char*) NULL){
						sv->readbuf[i]= (char*)malloc( READBUFSIZE ); /* $B=i4|%P%C%U%!(B */
						if( sv->readbuf[i] == (char*)NULL) continue;
						bzero( sv->readbuf[i] , READBUFSIZE );
						bcopy( buf ,sv->readbuf[i] , readlen );
						sv->readbuf_use[i] = readlen;
					} else {
						/* $B5l%P%C%U%!$,$"$k>l9g$O?7$K%3%T!<$7$F(Bappend */
						char *newbuf = (char*) malloc( newbuflen ) ;
						if( newbuf == (char*)NULL ) continue;    /* $B%a%b%j$?$j$s$+$C$?$i(Bdata$B<N$F$k(B */
											
						bcopy( sv->readbuf[i] , newbuf , sv->readbuf_use );
						bcopy( buf , newbuf + sv->readbuf_use[i] , readlen );  /* $B$3$3$G(Bnewbuf$B40@.(B*/
						free( sv->readbuf[i] );   /* $B%U%j!<$7$F%P%C%U%!F~$l$+$((B */
						sv->readbuf[i] = newbuf;
						sv->readbuf_use[i] += readlen;
					}
				} else {
					/* $B%5%$%:$r1[$($J$+$C$?$i$=$N$^$^%"%Z%s%I(B */
					bcopy( buf , sv->readbuf[i] + sv->readbuf_use[i] , readlen );
					sv->readbuf_use[i] += readlen;
				}

				/*$B%P%C%U%!$rJQ99$G$-$?$i!"(B1$B9T$=$m$C$F$k$+$I$&$+D4$Y$k!#2~9T5-9f$^$G$r%7%U%H(B */
				if( ( linelen =  checkLineEnough( sv->readbuf[i] , sv->readbuf_use[i])) ){
					int ll;


					/*$B2~9T5-9f$r%L%kJ8;z$K$7$F$+$i8F$S$@$7(B */
					sv->readbuf[linelen-1] = '\0';
					
					tsrpcServerDispatcher( sv->readbuf[i] , i );
					for(ll=0;ll<linelen;ll++){
						sv->readbuf[i][ll] = sv->readbuf[i][ll+linelen];
					}
					sv->readbuf_use[i]-=linelen;
				}
			}
		}

		/* UDP
		if( FD_ISSET( sv->udp_sockfd , &rfds ) ){
		}

		/* $B%k!<%W4X?t$r8F$S$@$9(B */
		if( sv->loopfunc) sv->loopfunc();

	}
}
/************
   $B%5!<%P!<$rGK2u$9$k(B

************/
void tsrpcDestroyServer( SERVER*s )
{
	int i;
	close( s->tcp_sockfd );	
	close( s->udp_sockfd );
	for(i=0;i<MAXSOCKFD;i++){
		if( s->sockuse[i] != 0 ){
			close( i );
		}
	}
}

/************
  $B%P%C%U%!$K(B1$B9TJ,0J>e$=$m$C$F$$$k$+$I$&$+$7$i$Y$k(B
  return value : 0 $B$=$m$C$F$J$$(B 1 $B$=$m$C$F$k(B
  
************/
int checkLineEnough( char *buf , int max )
{
faskjdhfksjahf	
}
