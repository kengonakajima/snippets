/*

  $B$3$N%3!<%I$b(Brpcgen$B$,EG$-=P$9!#(B

  $B4X?t$NFbMF$rDj5A$7$F$$$k$N$@!#(B







*/

#define _TSRPC_CLIENT_C_

#include "tsrpc.h"
#include "tsrpc_client.h"      /* $B$3$l$O4X?t$N%W%m%H%?%$%W$,F~$C$F8@$k!#(B */




int remotesum2( CLIENT *ch, char *arg1 , int arg2[10] )
{

/* $B%a%b%j3NJ]%U%'!<%:(B */
	char *buf1 = (char*)malloc( MKSTR_MINSIZE );
	char *retbuf = (char*)malloc( MKSTR_MINSIZE );
	char *callbuf = (char*)malloc( MKSTR_MINSIZE );


/* $BJ8;zNs:n@.(B */
	callbuf = joinString( callbuf , "remotesum2" );
	callbuf = joinString( callbuf , mkstr_string( arg1 ) );
	callbuf = joinString( callbuf , buf1 = mkstr_int32_array( buf1 , 10 , arg2 ) );
	callbuf = joinString( callbuf , "\n" );

/* $B8F$S$@$7%U%'!<%9(B */
	call_tsrpctcp( ch ,callbuf , retbuf );

/* $B%a%b%j2rJ|%U%'!<%:(B */
	free( (char*)retbuf );
	free((char*) buf1 );
	free((char*) callbuf );

/* $BJV$jCMJQ49(B */

	return demkstr_int32(retbuf);
}









/************

  $B%/%i%$%"%s%H$r=i4|2=$9$k(B

*************/
CLIENT * tsrpcCreateClient( char *hostname , int port )
{
	struct sockaddr_in sin;
	struct hostent *h;
	struct in_addr ina;

    /* $B%a%b%j3NJ](B */
	CLIENT *ch = (CLIENT * ) malloc( sizeof( CLIENT ) );
	if( ch == NULL ){
		fprintf( stderr ,"cannot allocate CLIENT struct.\n" );
		return ch;	
	}

	/* $BA4It%<%m$K$9$k(B */
	bzero( ch , sizeof( CLIENT ) );

    /* $BJQ?t=i4|2=(B */
	ch->port = port;
	strcpy( ch->hostname , hostname );



	/* $B%"%I%l%9$rCN$k(B */
	h = gethostbyname( hostname );
	if( h == NULL ){
		fprintf( stderr , "cannot gethostbyname %s.\n" , hostname );
		return NULL;
	}
	bcopy( h->h_addr_list[0] , &ina.s_addr , 4 );
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr = ina;

    /* TCP/UDP $B%=%1%C%H:n@.(B */


	ch->tcp_sockfd = socket( AF_INET , SOCK_STREAM , 0 );
	if( ch->tcp_sockfd < 0 ){ 
		fprintf( stderr, "tcp socket() error.errno:%d\n" , errno );
		return NULL;
	}
	ch->udp_sockfd = socket( AF_INET , SOCK_DGRAM , 0 );
	if( ch->udp_sockfd < 0 ){
		fprintf( stderr , "udp socket() error.errno:%d\n" , errno );
		close( ch->tcp_sockfd );
		return NULL;
	}

	/* connect$B$G$-$J$+$C$?$i2?$b$G$-$J$$$N$GA4It%/%m!<%:$7$FJV$9(B */
	if( connect( ch->tcp_sockfd , (struct sockaddr *) &sin , sizeof( struct sockaddr_in))<0){
		close( ch->tcp_sockfd );
		close( ch->udp_sockfd );
		fprintf( stderr , "connect error.errno: %d\n" ,errno);
		return NULL;
	}

	/*  $BMW5a%?%$%`%"%&%H$N@_Dj(B */
	ch->timeout = 40;

	/* $B=`Hw40N;(B */
	strcpy( ch->errmsg , "" );
	return ch;
}

/*************
   $B%/%i%$%"%s%H$rGK2u$9$k(B

*************/
int tsrpcDestroyClient( CLIENT*ch )
{

	/* Close sockets */
	close( ch->tcp_sockfd );
	close( ch->udp_sockfd );

	/* free memory */
	free( ch );

	return 0;
}


/************
  $B%/%i%$%"%s%H$K4X$9$k%(%i!<$rF@$k(B

************/
char* tsrpcGetLastError( CLIENT *ch , char *err )
{
	if( err ) strcpy( err , ch->errmsg );

	return ch->errmsg;
}

/************
  $B%a%C%;!<%8$rAw?.$7$F7k2L$rF@$k(B

************/
void call_tsrpctcp( CLIENT *ch , char *callbuf , char *retbuf )
{
	/* $B=q$-$3$_$OC1=c(B */
	write( ch->tcp_sockfd , callbuf , strlen(callbuf ) );
	
	/* $BFI$_$3$_$O<+A0$N%i%$%V%i%j%k!<%A%s$r;H$&(B */
	tsrpcClientReadLine( ch , retbuf );

}
/************
  $B%P%C%U%!$5$l$?FI$_$3$_4X?t(B

  $B2DG=$J$+$.$jFI$_$3$s$G!"%P%C%U%!$K(B1$B9T0J>e$N%G!<%?$,$-$?$i$=$3$G(B
$B$d$a$F!"$=$l$@$1%7%U%H$7$F$"$H$O;D$7$F$*$/(B

   select$B$r$D$+$C$?%N%s%V%m%C%-%s%0BP1~$G$"$k!#(B($B%?%$%`%"%&%H@_Dj2DG=(B)

************/
void tsrpcClientReadLine( CLIENT *ch , char*retbuf )
{
	
	fd_set rfds;
	time_t storet;
	
	time( &storet );



	while(1){
		char buf[READBUFSIZE];
		int readlen , afterlen;
		int i;
		time_t nowt;

		/* fd_set $B$r:n$k(B */
		FD_ZERO( &rfds );
		FD_SET( ch->tcp_sockfd , &rfds );


		/* $B$R$?$9$i$+$i$^$o$j$9$k(B */
		select(  2 , &rfds , (fd_set*)NULL , (fd_set*)NULL , 0 );
		
		if( FD_ISSET( ch->tcp_sockfd , &rfds ) ){
			if( ( readlen = read( ch->tcp_sockfd , buf , sizeof( buf ) ) )<= 0 ){
				strcpy( retbuf , "" );
				sprintf( ch->errmsg , "read error. closed socket. errno:%d\n" , errno );
				close(ch->tcp_sockfd );
				return;
			}
			/* $BFI$_$3$s$@J,$r2C$($k!#$=$N$H$-$K%P%C%U%!$ND9$5$r1[$($?$iA}$d$9(B */
			afterlen = readlen + ch->readbuf_use;

			if( afterlen  > ch->readbufsize ){
				char *newbuf;

                /* $B2?$b3NJ]$7$F$J$+$C$?$i$^$:3NJ]$9$k(B */
				if( ch->readbuf == NULL ){
					ch->readbufsize = READBUFSIZE;
					ch->readbuf = (char*) malloc( ch->readbufsize );
					ch->readbuf_use = 0;
					if( ch == NULL ){
						strcpy( retbuf , "" );
						ch->readbufsize = 0;
						strcpy( ch->errmsg , "cannot malloc readbuf." );
						return;
					}
					printf( "allocated %d bytes.\n" , READBUFSIZE );
				}

				printf( "exceeded. read:%d buf:%d use:%d\n" ,
						readlen , ch->readbufsize , ch->readbuf_use );

				/* $B$"$?$i$7$$%a%b%j$r3NJ]$7$F$+$i%3%T!<$7$F(Bfree$B$9$k(B */
				newbuf = (char*)malloc( afterlen );
				if( newbuf == NULL ){
					strcpy( retbuf , "" );
					sprintf( ch->errmsg , "memory shortage." );
					return;
				}
				bcopy( ch->readbuf , newbuf , ch->readbufsize );
				/* $B$b$A$m$sFI$_$3$s$@J,$b2C$($s$H$M(B */
				bcopy( buf , ch->readbuf + ch->readbuf_use , readlen );
				free( ch->readbuf );

				/* $B%P%C%U%!$rF~$l$+$($k(B */
				ch->readbuf = newbuf;
			}

			/* $B%P%C%U%!$N@hF,$+$i$7$i$Y$F(B1$B9TJ,$=$m$C$F$k$+$I$&$+$7$i$Y$F$=$m$C$F$?$i(Breturn */
			for(i =0 ; ; i++ ){
				if( ch->readbuf[i] == '\n' ){
					/* $B$$$C$3$G$b$3$N5-9f$,$"$C$?$iB7$C$F$k$H$$$&$3$H$@(B*/
					bcopy( ch->readbuf , retbuf , i  );/* $B2~9T5-9f$NA0$^$G%3%T!<(B */
					retbuf[i+1] = '\0';   /* $B%L%kJ8;z$rIU2C(B */
					strcpy( ch->errmsg , "" );
					return;
				}
			}
		}

		/* $B;~4V$r$7$i$Y$F$"$-$i$a;~4V$K$J$C$?$i=P$F$^$&(B */
		
		time( &nowt );
		if( nowt > ( storet + ch->timeout ) ){
			strcpy( retbuf , "" );
			sprintf( ch->errmsg , "readline timeout(%dsec). " , ch->timeout );
			return;
		}
	}

	/* not reached */

}

