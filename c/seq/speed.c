#include <stdio.h>
#include <time.h>
#include <sys/time.h>


struct slot
{
	int use;
	
	unsigned long ip_net;
	unsigned short port_net;
	int dslot;					/* $BAj<j$N%9%m%C%H(B */
	
	unsigned int send_i;		/* $B=q$-$3$_HV9f(B $B:G8e$K(Breg$B$7$?%Q%1%C%H$N(B*/
	unsigned int recv_i;		/* $BFI$_$3$_HV9f(B $B<!$KFI$_$3$`%Q%1%C%H$N(B */

	int open_ack;				/* open$B$N(Back$B$,$-$?$i(B1 */
	int accept_wait;			/* accept$B$5$l$k$N$r$^$C$F$$$k(B */

	int close_wait;				/* close$B$5$l$k$N$r$^$C$F$$$k>uBV$J$i(B1$B!#(B
								   CLOSE$B%3%^%s%I$,$*$/$i$l$F$-$?$i(B1$B$K$7!"(B
								   $B$<$s$V$N%G!<%?$r$h$_$3$`$^$G$O<B:]$K$O(B
								   $B%/%m!<%:$7$J$$!#$3$N%U%i%0$,(B1$B$N>uBV$G(B
								   $BFI$_$3$a$k%G!<%?$b=q$-$3$a$k%G!<%?$b(B
								   $B$J$/$J$C$?$i!"(Buse$B$r(B0$B$K$9$k!#(B
								   close$B$G$-$k$+$b$7$l$J$$%?%$%_%s%0$O!"(B
								   CLOSE$B%3%^%s%I$,$-$?$H$-!"(B
								   CLOSEACK$B%3%^%s%I$,$-$?$H$-!"(B
								   rp_recv$B$7$?$H$-(B(recvq$B$r>C2=(B)$B!"(B
								   DATAACK$B$r$&$1$H$C$F(Bsendq$B$r>C2=$7$?(B
								   $B$H$-$G$"$k!#(B
								   $B$=$l$i$N$H$-$K%A%'%C%/$7$F(Bclose$B2DG=(B
								   $B$G$"$l$P!"(Bclose$B$9$k!#(B

								   */
	
	double last_in;				/* $B:G8e$K%Q%1%C%H$,$-$?$N$O$$$D$+(B */


#define QSIZE	16
	char sb[QSIZE][256];
	char rb[QSIZE][128];

	int open_pbi , close_pbi;	/* open , close$B$N%Q%1%C%H$rAw?.$7$?$i(B
								   $B$I$N%Q%1%C%H%P%C%U%!$r$D$+$C$?$+$3(B
								   $B$3$K5-21(B */
	
};

#define MAXSLOT 8000

struct slot* slot;





double getUTimeDouble(void)
{
	struct timeval tm;

	gettimeofday( &tm , NULL );

	return (double)( tm.tv_usec ) + (double)( tm.tv_sec) *
	1000000.0 ;
}




int main()
{
	int i;
	double starttime;
	unsigned long ipn;
	unsigned short portn;
	char buf[128];
	

	printf( "structsize: %d tital:%d\n", sizeof( struct slot ),
		   sizeof( struct slot ) * MAXSLOT );
	
	slot = (struct slot*) malloc( sizeof( struct slot ) * MAXSLOT );
	if( slot == NULL ) return -1;


	starttime = getUTimeDouble();
	for(i=0;i<100000;i++){

		int ind = rand() % MAXSLOT;
		
/*		printf( "%d\n" , ind );*/
		bcopy( buf ,slot[ind].rb[i%16] , sizeof(buf ));
	}
	
	fprintf( stderr , "TIME: %f msec\n" ,
			( getUTimeDouble() - starttime ) / 1000.0);

	
}

