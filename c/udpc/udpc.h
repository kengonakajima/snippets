#ifndef _UDPC_H_

#define _UDPC_H_

int UDPCInit(unsigned short port);
void UDPC(void);
int UDPCAccept(void);
int UDPCConnect( unsigned long ipaddr , unsigned short port );
int UDPCWrite( int vfd , char *buf , int len );
int UDPCRead( int vfd , char *buf , int len );
int UDPCClose( int vfd );


#define ENOLISTEN		909			/* listen$B$7$F$J$$$>(B */
#define ENOACCEPTWAIT	1010		/* accept wait$B$N>uBV$K$J$C$F$$$k%=%1%C%H$,$"$k$O$:$J$N$K$J$$!#(B */
#define ESOCKETERR		505			/* socket($B$G$-$J$+$C$?!#(B) */
#define ENOMORESOCKET	606			/* $B$3$l0J>e%=%1%C%H$r$D$/$l$J$$$>(B */
#define EBADFDNUM		101			/* fd$B$NCM$,JQ$G$"$k(B */
#define EBADDATALEN		202			/* $B%Q%1%C%HAw?.$G$-$k%G!<%?$N%5%$%:$r1[$($F$k$+Ii$J$N$GAw?.$7$J$+$C$?(B */
#define EBUFFERMAX		303			/* $B%=%1%C%H$K$?$a$k$3$H$,$G$-$J$+$C$?!#(B */
#define ETIMEOUT		404			/* timeout$B$7$F$7$^$C$?!#(B */
#define ENOTINIT		2020		/* $B$^$@=i4|2=$5$l$F$J$$$>(B */
#define EALREADYUSE		3030		/* $B$9$G$K$=$N%]!<%H$O$D$+$o$l$F$$$k!#(B */
#define EREADNODATA			4004		/* $B%G!<%?$,$J$$$N$K(Bread$B$7$h$&$H$7$?!#(B */




#endif	/* _UDPC_H_ */
