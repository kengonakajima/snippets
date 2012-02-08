
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/errno.h>
#include <sys/time.h>





/* $B%(%i!<HV9f(B */

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




int uproc(void);
int ureadable( int fd );
int uconnect( int fd , unsigned long addr , unsigned short port );
int usocket_serv(int index );
int uaccept( int fd , unsigned long *cliaddr,  unsigned short *cliport );
int uwrite( int fd , char *data , int len );
int uread( int fd, char *data , int len );
void ustat(void);

