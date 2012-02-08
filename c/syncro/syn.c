#define _SYN_C_

#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>


#include "syn.h"

#define IOCTLFUNC ioctl        /* win32$B$G$O$3$l$r(B ioctlsocket $B$KJQ99(B */

/*
 *
 *  $B%2!<%`$N%3!<%I$O4pK\E*$K$O(B
 *
 * 1 initSyncro$B$9$k(B
 * 2 addSyncroAddr$B$r$D$+$C$F<+J,0J30$NA40w$N%"%I%l%9$rEPO?(B
 * 3 negotiateSyncroHosts$B$r%k!<%W$5$;$F%[%9%H$rA4ItB7$($k(B
 * 4 $B%2!<%`3+;O!#$G(BdoSyncro()$B$9$k(B
 * 5 $B%2!<%`=*N;$G(BfinishSyncro()$B$9$k(B
 * 
 * 1$B$+$i(B5$B$r$/$j$+$($9!#(B
 *
 * $B$H$$$&$h$&$K$J$k$N$@!#(B
 * $B%[%9%HF1;N$O(B2$BK\$E$D!"(Bread$BMQ!"(Bwrite$BMQ$N%=%1%C%H$r$b$D!#(B
 * n$B%[%9%H$@$C$?$i(B1$B%[%9%H$"$?$j!"(B2(n-1)$BK\$N%3%M%/%7%g%s$r;}$D$3$H$K$J$k!#(B
 * $B$=$N%3%M%/%7%g%s$,A4It$=$m$C$F!"$O$8$a$F(BOK$B$H$J$k!#(B
 * negotiateSyncroHosts$B$O!"Aj<j%[%9%H$H$N(B2$BK\$,$=$m$&$H(Bnow$B$,(B1$BA}$($k!#(B
 */


SOCKET sockfd;            /* $B%a%$%s$N%=%1%C%H(B */
unsigned short port;      /* $B<+J,$N%]!<%H(B */
int current_session_id;                   /* $B%+%l%s%H(B $B%;%C%7%g%s(BID */
int current_my_id;                        /* $B@$3&$K$R$H$D$7$+$J$$<+J,$N(BId. */
BOOL initialized = FALSE;        /* $B=i4|2=$5$l$F$$$k$+$I$&$+(B */

struct sockaddr_in sockaddr;     /* $B<+J,$N%"%I%l%9(B */

#define MAXCON 128

typedef struct {
    int use;
    int sockfd;
    int init;                    /* sockfd $B$,0UL#$N$"$kCM$J$i(B1 */

    int type;               
#define CON_TYPE_READ  101   /* accept$B$7$F!"(Bread$B$@$1$9$k%=%1%C%H(B */
#define CON_TYPE_WRITE 102   /* connect$B$7$F!"(Bwrite$B$@$1$9$k%=%1%C%H(B */
    
    char asciiaddr[128];         /* $B%"%9%-!<J8;zNs$N%"%I%l%9!#(B  */
    unsigned short port;         /* $B%]!<%H!#%[%9%H%P%$%H%*!<%@!#(Bconnect$B$9$k$H$-$D$+$&(B */
    
    struct sockaddr_in peer;     /* $B%T%"%"%I%l%9!#(Baccept$B$9$k$H$-$D$+$&(B */

    int my_id;                   /* $BAj<j$N(Bmy_id. TYPE_READ$B$G$D$+$&!#$3$NCM$,$*$J$8(B
                                    $B@\B3$,(B2$B8D0J>e$-$?$i(B2$B8DL\0J9_$O40A4L5;k(B 
                                  */
    
    int negotiation_ok;          /* negotiation$B$,$&$^$/$$$C$?$+$I$&$+(B */
    
    
} CONNECTION;

CONNECTION con[ MAXCON ];

extern int errno;


static int findBlankConnection( void );
static void prepareAllConnections(void);
static void releaseConnection( int index );

/*
 * syncro $B$r$7$g$-$+$9$k!#(B
 * 
 * unsigned short port : $B$^$A$&$1%]!<%H(B. $B%[%9%H%P%$%H%*!<%@$G(B
 * int session_id : $B%;%C%7%g%s(BID
 * int my_id : $B<+J,$N(BID. $B@$3&$G$R$H$D$7$+$J$$!#%^%C%A%s%0$5!<$P%+%i%b%i%&!#(B
 *
 * return value:
 * -1 : $B$9$G$K=i4|2=$5$l$F$$$k$N$G!"@h$K(BfinishSyncro$B$7$F$[$7$$!#(B
 */
int initSyncro( unsigned short port , int session_id , int my_id )
{
    int ret;
    
	if( initialized ) return -1;

    /* ID $BN`$r5-21(B */
    current_session_id = session_id;
    current_my_id = my_id;
    

    sockfd = socket( AF_INET , SOCK_STREAM , 0 );

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons( port );
    sockaddr.sin_addr.s_addr = INADDR_ANY;

    ret = bind( sockfd ,(struct sockaddr *)&sockaddr,sizeof( struct sockaddr_in ));
	if( ret < 0){
		perror((char *)strerror(errno));
		exit(1);
	}

    ret = listen( sockfd , 5 );
	if(ret < 0){
		perror( (char *) strerror(errno) );
        exit(1);
	}
    
    initialized = TRUE;

    return 0;
}

/*
 * $B%j%b!<%H%[%9%H$N%"%I%l%9$rDI2C!#<+J,0J30$N$@$1!#<+J,$rDI2C$7$F$bL5;k$5$l$k(B($BDI2C$5$l$J$$(B)
 *
 * int my_id : $BAj<j$N(Bmy_id$B!#(B
 */
int addSyncroAddr( char *ipaddr, unsigned short port )
{
    int index;
    
    if( !initialized )return -1;

    index = findBlankConnection();
    if( index < 0 )return -1;


    /* $B9=B$BN$K>pJs$r%3%T!<$9$k(B */
    con[index].use = 1;
    strcpy( con[index].asciiaddr , ipaddr );
    con[index].port= port;
    con[index].type = CON_TYPE_WRITE;
    con[index].negotiation_ok = 0;
    con[index].init = 0;                /* $B$^$@(Bsocket()$B$7$F$J$$(B */
    con[index].my_id = current_my_id;

    fprintf( stderr , "added addr:%s port:%d\n" , ipaddr , port );
    return 0;
}

/*
 * $B%j%b!<%H%[%9%H$,B7$&$N$r$^$D!#(B
 *
 * $B$3$N4X?t$O%N%s%V%m%C%-%s%0$G!"%k!<%W$r$^$o$7$FBT$D!#(B
 *
 * int *total : $B9g7W$G$$$/$D$N%[%9%H$,$"$k$N$+!#(BaddSyncroAddr$B$7$?2s?t!#(B
 * int *now : $B8=:_$$$/$D$N%[%9%H$,(Bnegotiation$B40N;$7$F$$$k$+(B
 * 
 * return value:
 * 0 $B@5>o=*N;!#(B
 * $BIi(B $B%(%i!<!#$3$l0J>e=hM}$r$D$E$1$k$3$H$,$G$-$J$$$N$G!"(BfinishSyncro$B$7$?$[$&$,$h$$!#(B
 * total , now $B$,Ey$7$/$J$k$^$G!"<+J,$G;~4V$r$O$+$C$FBT$D$Y$7!#(B
 * $B$=$l$G!"$3$30J9_$G$O(Baccept$B$7$J$$$N$G!"$b$&%W%l%$%d!<$,A}$($k$3$H$O$J$$!#(B
 *
 * $B$d$k$3$H$O(B accept, connect$B$r%N%s%V%m%C%-%s%0$G$d$k$@$1$@!#(B
 * 
 */
int negotiateSyncroHosts( int *total, int *now )
{
    fd_set rfds , wfds ,efds;
    struct timeval tm;
    int i;
    
    if( !initialized ) return -1;


    /* $B=`Hw$G$-$F$$$J$$%=%1%C%H$,$"$C$?$i=`Hw$9$k(B*/
    prepareAllConnections();
        
    tm.tv_usec = 0;
    tm.tv_sec = 0;


    FD_ZERO( &rfds );
    FD_ZERO( &wfds );
    FD_ZERO( &efds );


    FD_SET( sockfd , &rfds );
    FD_SET( sockfd , &wfds );
    FD_SET( sockfd , &efds );

    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].init ){
            FD_SET( con[i].sockfd , &rfds );
            FD_SET( con[i].sockfd , &wfds );
            FD_SET( con[i].sockfd , &efds );
        }
    }
    
    select( MAXCON + 5 , &rfds , &wfds , &efds , &tm );

    /* $B%a%$%s%=%1%C%H$,$h$_$3$_2DG=$@$C$?$i!"%"%/%;%W%H$G$-$=$&$J$3$H$r0UL#$9$k(B */
    if( FD_ISSET( sockfd ,&rfds )){
        struct sockaddr_in peer;
        int peeraddrlen = sizeof( peer );
        
        int newsockfd = accept( sockfd, (struct sockaddr*)&peer , &peeraddrlen );
        if( newsockfd < 0 ){
            perror( (char*) strerror( errno ));
        } else {
            int index = findBlankConnection();
            if( index < 0 ){
                /* $B$b$&$3$l0J>e(Baccept$B$G$-$J$$(B($B0[>o;vBV(B )*/
            } else {
                con[index].use = 1;
                con[index].init = 1;
                bcopy( (char*)&peer , & con[i].peer ,sizeof(struct sockaddr_in));
                con[index].type = CON_TYPE_READ;
                con[index].sockfd = newsockfd;
                con[index].negotiation_ok = 0;
                con[index].my_id = 0;            /* $B$3$l$+$i$&$1$D$1$k$N$@(B */

#if 1
                {
                    fprintf( stderr,"accepted index:%d fd:%d\n",index,con[index].sockfd);
                }
#endif
            }
        }
    }

    /* $B%3%M%/%7%g%s$r%A%'%C%/(B */
    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].init ){
            char buffer[1024];
            
            if( FD_ISSET( con[i].sockfd , &rfds )){
                if( con[i].type == CON_TYPE_READ ){
                    int ret =  read( con[i].sockfd , buffer , sizeof( buffer ));
                    if( ret <= 0 ){
                        /* $B$=$N%=%1%C%H$O$b$&$@$a$@$3$j$c(B */
                        releaseConnection( i );
                        continue;
                    } else {
                        /* $B2?$+FI$_$3$a$?$>!#%M%4%7%(!<%7%g%s$N>pJs$+$I$&$+$7$i$Y$k(B */
#if 1
                        buffer[ret] = 0;
                        fprintf( stderr, "Read data:[%s]\n", buffer );
#endif
                    }
                }
            }
            if( FD_ISSET( con[i].sockfd , &wfds )){
                if( con[i].type == CON_TYPE_WRITE ){
                    
#if 0
                    /* $B$D$$$K(B connect$B$G$-$?$+$N%A%'%C%/$,$O$$$k$O$:$@$,(Bwindows$B$H$O(B
                     $B$d$j$+$?$,$3$H$J$k$h$&$@!#$J$N$G(BPENDING */
                    fprintf( stderr, "connect OK? on index:%d fd:%d\n", i,con[i].sockfd);
/*                    strcpy( buffer , "fucking!" );
                    write( con[i].sockfd ,buffer , strlen( buffer ));*/
#endif
                }
                       
            }
            if( FD_ISSET( con[i].sockfd , &efds )){
                if( con[i].type == CON_TYPE_READ ){
                    fprintf( stderr , "Fucking Exception on index:%d fd:%d\n" ,
                             i , con[i].sockfd );
                }
            }
        }
            
    }

    /* $B8=:_$N>u67$r=PNO(B */
    *total = *now = 0;
    
    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].init ){
            (*total)++;
            if( con[i].negotiation_ok ){
                (*now)++;
            }
        }
    }
    
    return 0;
}

/*
 * $B$b$&$D$+$o$J$$!"$D$+$($J$$!"%=%1%C%H$r2rJ|$9$k(B
 *
 * int index : $B$I$N%3%M%/%7%g%s$r2rJ|$9$k$N$+(B
 */
static void releaseConnection( int index )
{
    con[index].use = 0;
    close( con[index].sockfd );
    bzero( & con[index] , sizeof( con[0] )) ;

#if 1
    fprintf( stderr , "Released connection index:%d\n" , index );
        
#endif
}

/*
 * $B$9$Y$F$N%3%M%/%7%g%s$r(Bsocket()$B$9$k(B. sockfd$B$,0UL#$N$"$kCM$K$J$k$h$&$K$9$k!#(B
 * $B$b$A$m$s!"(Bconnect$B$9$k%?%$%W$N$d$D$@$1!#(B
 *
 *
 */
static void prepareAllConnections(void)
{
    int i;

    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].type == CON_TYPE_WRITE &&
            con[i].init == 0 ){
            int flg;
            int ret;
            SOCKET s;
            struct sockaddr_in sin;


            
            s = socket( AF_INET , SOCK_STREAM , 0 );

            flg = 1;

#if 0
            /* $B%N%s%V%m%C%-%s%0$K$9$k(B */
            IOCTLFUNC( s , FIONBIO , &flg );
#endif
            
            sin.sin_family = AF_INET;
            sin.sin_port = htons( con[i].port );
            sin.sin_addr.s_addr = inet_addr( con[i].asciiaddr );

            ret = connect( s ,(struct sockaddr*)&sin , sizeof(sin ));

            if( ret >= 0 ){
                con[i].init = 1;
                con[i].sockfd = s;
                fprintf( stderr , "Connect OK on index:%d" , i );
            } else {
                close( s );
                fprintf( stderr , "Connect fail on index:%d" , i );
                perror( (char*)strerror(errno));
            }
            
            /*
             * PENDING
             * 
             * $B$3$3$N%3!<%I$O!"(Berrno$B$r$_$F(BEINPROGRESS$B$+$I$&$+$r$7$i$Y$kI,MW$,$"$k$,!"(B
             * windows$B$G$bF0$/%3!<%I$K$9$k$K$O$I$&$9$k$+G:$`$N$GCV$$$F$"$k(B
             *
             * $B$^$?!"(BUNIX$B$G(Bconnect$B$r%N%s%V%m%C%-%s%0$K$9$kJ}K!$,$o$+$i$J$$$N$G!"(B
             * $B%V%m%C%/$9$k$^$^$K$7$F$"$k!#(B
             */
            

        }
    }
    
}


/*
 * $B%3%M%/%7%g%s$rJD$8$k!#(B
 * char *ipaddr : $BBP>]%j%b!<%H%[%9%H$N(BIP$B%"%I%l%9(B
 * unsigned short port : $B%]!<%H!#(B
 * 
 * return value:
 * -1 : $B=i4|2=$5$l$F$J$$$+$i2?$b$7$J$$$>!#(B
 * 
 * $B@\B3$rJD$8$k$,!"$=$N$H$-$K%"%I%l%9$H%]!<%H$r8!:w$7$F!"$9$G$K$R$i$$$F$$$k(B
 * $B$b$N$N$&$AN>J}$,0lCW$9$k$b$N$@$1JD$8$k!#(B
 */
int closeSyncroAddr( char *ipaddr , unsigned short port )
{
    int i;
    
	if( !initialized )return -1;

    for(i=0;i<MAXCON;i++){
        if( con[i].use && con[i].init ){

        }
    }
    

    return 0;
}

/*
 * $B%@%V%k%P%C%U%!$r(Bdiff$B$7$F!"Aw?.$9$k$Y$-$b$N$r$5$,$7$@$7!"Aw?.$9$k!#(B
 *
 */
void doSyncro( void )
{
	if( !initialized )return;

}

/*
 *  $B%i%$%V%i%j$NF0:n$r=*N;$9$k!#$U$?$?$S$O$8$a$k$K$O!"(BinitSyncro$B$9$kI,MW$,$"$k$N$@!#(B
 *
 */
void finishSyncro( void )
{


	initialized = FALSE;

}

/*
 * $B%G!<%?$r$D$$$+$9$k!#(B
 * char *name : $BJQ?tL>!#J8;zNs$J$i2?$G$b$$$$$,!"(B
 * C$B$G$D$+$($kL>A0$=$N$^$^$+!"$=$l$r$_$8$+$/$7$?$b$N$,?d>)$5$l$k!#(B
 * void *pointer : $BJQ?t$N<BBN$r;X$9%]%$%s%?!#(B
 * TYPE type : $BJQ?t$N7?!#(B "int" , "short" , "char" , "string" $B$N(B4$B<oN`$N$_!#(B
 * "string"$B$N>l9g$O(Bcount$B$NCM$,(B1$B0J30$@$C$?$i%(%i!<!#J8;zNs$NG[Ns$O%5%]!<%H$7$J$$!#(B
 * int diff : $B:9J,Aw?.%?%$%W$N>l9g(B1$B!"$=$&$G$J$$>l9g(B0
 * int local : 1$B$J$i%M%C%H%o!<%/$+$i$NMW5a$G$OCM$rJQ99$7$J$$!#(B0$B$J$i$9$k!#(B
 *
 * $B$=$l$>$l$N%G!<%?$O!"C/$,:G8e$K%"%/%;%9$7$?$+!"$r5-21$9$k!#%A!<%HBP:vMQ(B 
 */
int addSyncroData(
 char *name , void *pointer , int count , char *type, int diff , int local )
{
	if( !initialized ) return -1;


    return 0;
}

/*
 * $B%m%0$r=PNO$9$k!#(B
 *
 */
void printSyncroLog( char *out , int maxlen )
{
	if( !initialized ){
		strncpy( out , "not initialized" , maxlen );
		out[maxlen]='\0';
		return;
	}


}
/*
 * rhost$B$N$"$-$r$5$,$9(B
 *
 * return value:
 * >=0 $BCM(B
 * <0 Error
 */
static int findBlankConnection( void )
{
    int i;
    for(i=0;i<MAXCON;i++){
        if( con[i].use == 0 ){
            return i;
        }
    }
    return -1;
}


/*
 * $BJQ?t$N%G!<%?$r(Bchar$B$NG[Ns$K$9$k!#(B
 *
 * short i : $BCM(B
 * char *out : $B=PNO(B
 * char *name : $BJQ?tL>(B
 * return value
 * $B$G$-$?$b$N$ND9$5(B(byte)
 * 
 */
int makeShortData( short i , char *out ,char *name )
{
    int nlen = strlen( name );
    
    strcpy( out , name );
    out[nlen]=0;
    out[nlen+1]='s';
    i = htons(i);
    bcopy( (char*)&i, &out[nlen+2] , 2 );

    return nlen + 4;
    
}
int makeShortDataDiff( short before , short after , char *out , char *name )
{
    int nlen = strlen( name );
    short i;
    
    strcpy( out , name );
    out[nlen]=0;
    out[nlen+1]='y';

    i = htons( ( after - before ) );
    bcopy( (char*)&i , &out[nlen+2] , 2 );

    return nlen + 4;
}

int makeShortArrayData( short *array , int sind , int eind , char*out , char *name )
{
    int i;
    char tmp[128];
    int silen , eilen;
    int nlen = strlen( name );

    strcpy( out , name );
    out[nlen]=0;
    sprintf( tmp , "%d" , sind );
    silen = strlen( tmp );
    strcpy( out + nlen + 1, tmp );
    sprintf( tmp , "%d" , eind );
    eilen = strlen(tmp );
    strcpy( out + nlen + 1 + silen + 1 , tmp );
    out[nlen+1+silen+1+eilen] = 'S';
    
    for(i=sind;i<=eind;i++){
        short s;
        s = htons( array[i] );
        bcopy( (char*)&s , out+nlen+1+silen+1+eilen+1+((i-sind)*2) , 2 );
    }
    return nlen+1+silen+1+eilen+1+((eind-sind+1)*2);
}


int makeTextData( char *text , char *out , char *name )
{
    char tmp[128];
    
    int nlen = strlen( name );
    int slen = strlen( text );
    int dlen;
    
    strcpy( out , name );
    out[nlen]=0;

    sprintf( tmp , "%d" , slen );
    dlen = strlen( tmp );
    
    strcpy( &out[nlen+1] , tmp );
    out[nlen+1+dlen]='T';
    strcpy( &out[nlen+1+dlen+1] , text );
    return nlen + 1 + dlen + 1 + slen;
}
