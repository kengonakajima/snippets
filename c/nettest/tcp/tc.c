/*


   	TCP$B$N@-G=%F%9%H(B


	$BJ#?t$N%=%1%C%H$+$i$NF~NO$r<u$1$D$1$k$?$a$K(Bselect$B$r;H$&!#(B
	
  	$B$7$+$7!"$=$l$G$b(B5$B$D$^$G$N@\B3$7$+$G$-$J$$!#(B

	$B%2!<%`$K$*$$$FAw?.$5$l$k%G!<%?$NNL$O!"(B

	$B%9%/%m!<%k$7$?$H$-$N:9J,(B ( 20 x 2 = 40byte$B$/$i$$(B)
	$BB>$N%-%c%i$N0LCV$J$I(B ( $B0l8D(B6byte$B$H$7$F!"(B $B%-%c%i$N?tJ,$@$1!#%-%c%i%*!<%P!<$r$9$k$h$&(B
	$B$K@_Dj$9$k$H$7$?$i!"$@$$$?$$(B10$B8D$^$G$/$i$$$+!#(B )
	$B<+J,$NBNNO$J$I$N>pJs(B(20byte$B$/$i$$!)(B)
	$B%F%-%9%H%G!<%?(B( max 100byte? )

	$B$@$$$?$$!"(B200byte$B$/$i$$$r8+$F$*$/!#(B

	64kbps$B$@$C$?$i!"(B10$B$G3d$C$F(B6.4Kbyte$B$/$i$$Aw$l$k$3$H$K$J$k!#(B32$B?M$G$a$$$C$Q$$$+!#(B
	$B$^$"!"$=$l$r1[$($?$iCY$/$J$C$F$b$7$+$?$,$J$$!#(B


	$B$$$/$D$b$N@\B3$rF1;~$K$G$-$k(BUDP$B$r;H$$$?$$!#(BFreeWalk$B$b(BUDP$B$r;H$C$F$$$k$3$H$@$7!#(B
	UDP$B$N?.Mj@-$r9b$a$k$?$a$K!"(BRTT$B%"%k%4%j%:%`$r;H$&!#$&$^$/:FAwMW5a$r=P$7$F$$$/!#(B


	P.490
*/

#define _T_C_

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>


#define HOST "murasaki"
#define MYECHO_SERVICE "myecho"

#define MAXLINE 255

char *pname;
extern int errno;




int main( int argc , char **argv )
{
	int sockfd ;

	pname =argv[0];

	if( ( sockfd = udp_open( HOST , MYECHO_SERVICE , 0 , 0 ) ) < 0 ){
		perror("udp_open error. " );
	}
	
	doit( stdin , sockfd );

	exit(0);

}

#define MAXMESG 2048

/*
   $B%(%3!<%k!<%A%s!#(B
   $B%G!<%?%0%i%`%/%i%$%"%s%H$N=*N;$O8!=P$9$k$3$H$,$G$-$J$$$N$G!"$3$N4X?t$O8F$S=P$7(B
   $B85$K!"=hM}$rJV$5$J$$!#(B
*/

void dg_echo( int sockfd , struct sockaddr *pcli_addr , int maxclilen )
{
	int n , clilen;
	char mesg[MAXMESG];

	for(;; ){
		clilen = maxclilen;
		n = recvfrom( sockfd , mesg , MAXMESG , 0 , pcli_addr, &clilen );
		if( n < 0 ){
			perror("dg_echo: recvfrom error.");
		}
		if( sendto( sockfd , mesg , n , 0 , pcli_addr , clilen ) != n ){
			perror("dg_echo: sendto error." );
		}
	}
	

}


#if 0
void doit(FILE *fp , int sockfd )
{
	int n , sendlen;
	long seqsend;
	char sendline[MAXLINE], recvline[MAXLINE];

	seqsend = 0;
	while( fgets( sendline + sizeof(long ) , MAXLINE - sizeof(long) , fp ) != NULL ){

		seqsend++;	/* $B=g=xHV9f$rA}2C$5$;$k(B */
		bcopy(( char * ) &seqsend , sendline , sizeof( long ) );
		sendlen = strlen( sendline + sizeof( long )) + sizeof( long );

rexmit:
		if( ( n = dgsendrecv( sockfd , sendline , sendlen , recvline , MAXLINE ,
				     ( struct sockaddr *) 0 , 0 )) < 0 ) {
			if( errno == EINTR )
				perror("client: no response from server" );
			else
				perror("client: dgsendrecv error");
		}

		if( bcmp(( char *) &seqsend , recvline , sizeof(long )) != 0){
			perror("incorrect sequence number received . ");
			goto rexmit;
		}
		recvline[n] = 0;	/* NULL character */
		fputs( recvline + sizeof( long ) , stdout );

	}
	if( ferror(fp ) ){
		perror("client: error reading file" );
	}
		


}



#endif
