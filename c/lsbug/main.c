2#define _MAIN_C_



#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "general.h"
#include "proto.h"
#include "main.h"
#include "log.h"

#include "npcengine.h"
#include "settingchar.h"
#include "configfile.h"
#include "chardata.h"
#include "itemfile.h"
#include "floor.h"
#include "fbuf.h"
#include "do.h"
#include "item.h"
#include "util.h"
#include "init.h"
#include "admin.h"
#include "httpd.h"
#include "floorfile.h"

#if SOLARIS25
#include <sys/file.h>
#include <listen.h>

#include <sys/sockio.h>
#include <sys/socketvar.h>

#include <arpa/nameser.h>
#include <resolv.h>


#endif


double time_keeper_1=0.0;		/* loop_speed$B$rM-8z$J$b$N$K$9$k(B */
double time_keeper_2=0.0;
double time_keeper_3=0.0;
double time_keeper_4=0.0;




int main( int argc , char **argv )
{

    int main_pid;

	daemon_mode = OFF;
	auto_boot = OFF;
    if( parseArgs( argc , argv ) < 0 ){

		logger("Usage: lserv [configfile] [-d] [-autoboot]\n"
			   "options:\n"
			   "-d : run lserv as a daemon.\n"
			   "-autoboot : automatic reboot lserv when it is down.\n"
			   "lserv is programmed by Kengo Nakajima (1997)\n");

		exit(0);

    }
		
	if( auto_boot ==ON ){
	    for(;;){
			main_pid = fork();
			if( main_pid == 0 ){
				server_main( argc , argv );

				/* not reached */
			} else {
				int stat;
				waitpid( main_pid , &stat , 0 );
				writeRebootLog(0);
				logger("Server down! reboot after 100 second!\n" );

				sleep(100);
			}
	    }
	} else {
	    server_main( argc , argv );

		/* not reached */
	}

	return 0;
}

void sig_restart(int in )
{
	

    int i;

    logger_int( "received SIGNAL:" , in );

    /* logout */
	
    for( i = 1 ; i < maxchar; i++ ){
		charLogout( i ,"signal-restart");
    }
	
    /* save */

    for(i = 0 ; i < maxchar ; i++ ){
		if( chardata[i].aliveflg == ON ){
			writeCharacterFile( i ) ;
		}
    }

    for(i=0;i<maxconnection;i++){
		if( fdlist[i] == ON) closeSocket( i );
    }
	

	/* $B%"%$%F%`%G!<%?=q$-$3$_(B */
	readWriteFloorItemData( 1 );

	writeRebootLog(in);
    logger("SERVER SHUTDOWN! Closed all sockets.\n");

    exit(0);
	
}

int server_main( int argc , char **argv )
{
    int newsockfd , clilen;
    struct sockaddr_in cli_addr , serv_addr;
	int select_any_connection;	
    int i;

    /* host name */

#if SOLARIS25
	if( gethostname( hostname , 256 ) == -1 ){
#else
    if( gethostname( hostname , &i ) == -1 ){
#endif
		logger("lserv: gethostname failed.\n");
		exit(0);
    }
	
    defaultSettings();
	
    readConfigFile();

	


    if( daemon_mode == ON) daemon_start( ON );

    initAll();

    signal( SIGSEGV , sig_restart );
    signal( SIGHUP , sig_restart );
    signal( SIGINT , sig_restart );
    signal( SIGQUIT , sig_restart );
    signal( SIGILL , sig_restart );
    signal( SIGTRAP , sig_restart );
    signal( SIGABRT , sig_restart );
    signal( SIGFPE , sig_restart );
    signal( SIGBUS ,sig_restart );
    signal( SIGSYS ,sig_restart );
    signal( SIGPIPE , SIG_IGN );
    signal( SIGALRM , sig_restart );
    signal( SIGTERM , sig_restart );
    signal( SIGURG , sig_restart );
    signal( SIGIO , sig_restart );
    signal( SIGXCPU , SIG_IGN );
    signal( SIGXFSZ , SIG_IGN );
    signal( SIGVTALRM , SIG_IGN );
    signal( SIGPROF , SIG_IGN );
    signal( SIGWINCH , SIG_IGN );
	
#if !SOLARIS25
    signal( SIGLOST , SIG_IGN );
#endif
	
    for(i = 0 ; i < maxconnection ; i++){
		fdlist[i] = OFF;
		connectmode[i] = CONNECTION_NOTDETECT;
    }

    if( ( mainsockfd = socket( AF_INET , SOCK_STREAM , 0)) < 0){
		logger_int("lserv: socket() error. errno=" , errno);
		exit(0);
    }
	
    fdlist[mainsockfd] = ON;
    connectmode[mainsockfd] = CONNECTION_MAIN;
    indextochar[ mainsockfd ] = 0;
	
    bzero((char *) &serv_addr , sizeof( serv_addr ));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if( bind( mainsockfd , (struct sockaddr *) &serv_addr , sizeof( struct sockaddr))<0){
		
		logger("lserv: bind error.\n");
		exit(1);
    }

    listen( mainsockfd , 5 );
    logger_int( "start loop. main socketFD=" , mainsockfd );
	logger_int( "Port: " , port );

	serv_ipaddr = serv_addr.sin_addr.s_addr;
	logger_int( "IPaddr: " , serv_ipaddr );
	
	/* $B%k!<%W$r3+;O$5$;$k$?$a$K$O!"(Btime_keeper_1$B$K:G=i$K@5$7$$;~4V$rF~$l$kI,MW$,$"$k(B */
	time_keeper_1 = getUTimeDouble();
	
    for(;;){
		int n , fdnum;
		fd_set fdv,rfdv,wfdv;
		struct timeval tmsel;

		/* $B%?%$%`%"%&%H=hM}(B */

		for(n=0;n<maxconnection;n++){
			if( lastmessage_when[n] < (systemtime-timeout_sec ) && lastmessage_when[n] != 0){
				/* $B%?%$%`%"%&%H$9$k$h$&$JE[$O!"(Blinger$B$7$?$k$>(B */
				struct linger lin;
				
				lin.l_onoff = ON;
				lin.l_linger = 0;
				setsockopt( n , SOL_SOCKET, SO_LINGER, (char *) &lin , sizeof( struct linger ));

				if( CALIVE( indextochar[n]) ){
					charLogout( indextochar[n] ,"timeout");
				}
				closeSocket( n );
				logger_int( "TIMEOUT: fd=" , n );
			}
		}

		/* $B%M%C%H%o!<%/1[$7$N%W%l%$%d!<$NA`:n(B,HTTP$BMW5a$r=hM}(B */

		tmsel.tv_sec = 0;
		tmsel.tv_usec = loop_speed;

		FD_ZERO( &fdv );
		FD_ZERO( &rfdv );
		FD_ZERO( &wfdv );

		for(i = 0 , fdnum = 0 ; i < maxconnection ; i++){
			if( fdlist[i] == ON ){
				FD_SET( i , &fdv);
				FD_SET( i , &rfdv );
				FD_SET( i , &wfdv );
				fdnum++;
			}
		}

		time_keeper_2 = getUTimeDouble();
		
		if( (int)( time_keeper_2 - time_keeper_1 ) > ( loop_speed  ) ){
			/* $B<B:]$N=hM}$,%k!<%W%9%T!<%I$h$j$b1[$($F$7$^$C$F$$$k!#(B */
			too_much_work = ON;
			tmsel.tv_sec = 0;
			tmsel.tv_usec = 1;
		} else {
			/* $B=hM}$,%k!<%W%9%T!<%I$K$*$$$D$$$F$$$k!#(B */
			double tmpd;

			tmpd = loop_speed  - (int)( time_keeper_2 - time_keeper_1 ) ;
			tmsel.tv_sec = (int)(tmpd / 1000000.0);
			tmsel.tv_usec= (int)(tmpd - (double)tmsel.tv_sec * 1000000.0 );
		}
		
		select( maxconnection , &rfdv , &wfdv , (fd_set*)0,&tmsel );

		time_keeper_3 = getUTimeDouble();
		
		/* select$B$b$9$0$K=*$o$C$F$7$^$C$?$i!"(Busleep$B$9$k!#(B */
		if( (int)( time_keeper_3 - time_keeper_1 ) < (loop_speed  )  ) {
			int sleep_time = loop_speed - ( time_keeper_3 - time_keeper_1 ) ;
			usleep( sleep_time );
		}

		time_keeper_1 = getUTimeDouble();

		/* $B$3$N9T$+$i!"<!$N(Bselect$B$^$G$O!"(Bread/write$B0J30%N%s%9%H%C%W$G$"$k!#(B
		   $B$=$N=hM}$K(Bn msec$B$+$+$k$H$9$k$H!"$D$M$K(Bloop_speed msec$B$G$9$9$s$G$[$7$$$N(B
		   $B$@$+$i!"(B n > loop_speed$B$N;~$O2?$b$;$:!"(B n < loop_speed$B$N;~$O(B
		   select$B$N;~4V$K(B n-loop_speed$B$rBeF~$7!"$5$i$K;~4V$r7WB,$7!"(B
		   select$B$b$9$0$K$*$o$C$?$i$^$@B-$j$J$$J,$r(Busleep$B$G$^$D!#(B
		*/
		
		/* $BFI$_$3$a$k$b$N$,$"$C$?$iFI$s$G%P%C%U%!$KF~$l$k!#(B */
		select_any_connection=OFF;
		for(i = 0 ;i < maxconnection ; i++){
			if( FD_ISSET( i , &rfdv )){
				select_any_connection=ON;
				if( i == mainsockfd ){
					clilen = sizeof( cli_addr  );

					newsockfd = accept( mainsockfd , (struct sockaddr *) &cli_addr , &clilen );
										
					if( newsockfd < 0 ){
						logger("lserv: accept error.\n");
						continue;
					}

					fdlist[ newsockfd] = ON;
					lastmessage_when[newsockfd] = systemtime;
					connectmode[newsockfd] = CONNECTION_NOTDETECT;
					

					/* $B%N%s%V%m%C%-%s%0$K$9$k(B */
					fcntl( newsockfd , F_SETFL , FNDELAY );

					client_ipaddr[newsockfd] = ntohl( cli_addr.sin_addr.s_addr );
					client_port[newsockfd] = ntohs( cli_addr.sin_port );
				} else {
					
					int ret;
					char tmpbuf[ RWUNIT+1];
					ret = read( i , tmpbuf , RWUNIT );
					
					/* $BFI$_(BOK$B$J$N$K(B0$B$rJV$9$?$I$&$$$&$3$H$@(B */
					if( ret <= 0 ){
						if( getFWriteBufLength(i) == 0 ){
							/* $B%-%c%i$,%m%0%$%s$7$F$$$k;~$O!"B(:B$K%m%0%"%&%H$5$;$k!#(B */
							charLogout( indextochar[i] , "readerror");
							closeSocket(i);
							sprintf(tmpbuf , "closed(readerror).fd=%d " , i );
							logger( tmpbuf );
						}
					} else {
						
						rawlog( socketinputlog , tmpbuf , ret ,OFF);
						
						tmpbuf[ret]='\0';

						appendFReadBuf( i , ret , tmpbuf );
						readbytes += (double) ret;
						readbytes10sec += (double) ret;
						lastmessage_when[i] = systemtime;

					}
				}
			}
		}
		  
		/* $B=hM}$9$k!#(B */
		for(i = 0 ; i < maxconnection ; i++){
			if( i == mainsockfd ) continue;
			if( fdlist[i] == ON ){
				if( getFReadBufLength(i) > 0 ){
					switch( connectmode[i] ){
						case CONNECTION_NOTDETECT:
						detectMode( i );

						/* $B%2!<%`$d4IM}MQ$N@\B3$O!"(BLINGER$B$7$F$*$/I,MW$,$"$k(B */

						if( connectmode[i] == CONNECTION_GAME ||
						   connectmode[i] == CONNECTION_ADM ){
							int flgval = 1;
							struct linger lin;
							lin.l_onoff = ON;
							lin.l_linger = 0;
							
							setsockopt( i , SOL_SOCKET, SO_LINGER, (char *)&lin , sizeof( struct linger ));
							setsockopt( i , IPPROTO_TCP , TCP_NODELAY , (char *) &flgval , sizeof( int ));
						}
						/* $B@_Dj$K$h$C$F$O!"@\B3$,$G$-$?$i$9$0$K!"2hA|A4It$rAw$k(B */
						if( !send_image_after_login_ok && connectmode[i] == CONNECTION_GAME){
							sendButtonImages(i);
						}

						break;

						case CONNECTION_HTTP:
						/* $B$"$H$OAw$k$@$1$d$F(B */
						break;
						
						case CONNECTION_GAME:
						case CONNECTION_ADM:
						
						doIt( i );
						break;
					
						default:
						logger("illegal!");
						closeSocket(i);
						break;
					}
				}
			}
		}
		
		/* $B%P%C%U%!$K$?$^$C$F$$$F!"$+$D=q$-$3$a$k$b$N$,$"$C$?$i%P%C%U%!$+$i(B
		   $B=P$7$F$-$FF~$l$k(B */
		for(i = 0 ; i < maxconnection ; i++){
			if( i == mainsockfd ) continue;
			if( FD_ISSET( i , &wfdv ) && getFWriteBufLength(i) > 0){
				
				int ret;

				/* $B=q$-$3$`!#JD$8$k$[$&$,@h$J$N$O!"$b$&0l<~2s$7$F;~4V$r$+$;$0$?$a(B */
				if( getFWriteBufLength( i) > 0 ){
					char msg[512];
					sprintf( msg , "WR: i:%d l:%d cM:%d Ci:%x Cp:%d Ind:%d \n" ,
							i , getFWriteBufLength( i ) , connectmode[i] , client_ipaddr[i] ,
							client_port[i] , indextochar[i]  );
					logger( msg );

					logger("<");
					
					ret = write( i , fbuf[i].writebuf , getFWriteBufLength(i));
					if( connectmode[i]==CONNECTION_GAME &&
					   getFWriteBufByte(i , 0 ) != 'I' && getFWriteBufByte(i,1)!='M' &&
					   debug_mode == ON){
						rawlog( socketoutputlog , fbuf[i].writebuf , ret,ON);
					}

					logger(">" );

					
					writebytes += (double) ret;
					writebytes10sec += (double)ret;
					
					shiftFWriteBuf( i , ret );
				}

				/* $B=q$-$3$_$,=*$o$C$?$iJD$8$k(B */
				if( getFWriteBufLength( i ) == 0 ){
					if( getFCloseFlg( i ) == ON ){
						closeSocket( i );
					}
				}
				
			}
		}
		/* $B;W9M%k!<%A%s(B */
		engine();

		/* $B%7%9%F%`$+$s$j(B */
		systemAdmin();
    }
}
/***************************
  $B%=%1%C%H$rJD$8$k(B
  return value ... 0 ... OK
  <0 ... Error
  ***************************/
int closeSocket( int sockfd )
{
	if( sockfd < 0 || sockfd >= maxconnection ) return -1;

	chardata[ indextochar[sockfd]].onlineflg = OFF;
    fdlist[ sockfd ] = OFF;
    indextochar[ sockfd ] = 0;
    connectmode[ sockfd ] = CONNECTION_NOTDETECT;
	lastmessage_when[sockfd] = 0;
	client_ipaddr[sockfd] = 0;
	client_port[sockfd] = 0;
    close( sockfd );
    clearFBuf( sockfd );
    setFCloseFlg( sockfd ,OFF);

	return 0;
}
/**************************
  lserv$B$X$N0z?t$r$7$i$Y$k(B

**************************/
int parseArgs( int argc , char **argv )
{
    int i;
	int configfile_specified = OFF;
    strcpy( settingfilename , CONFIGFILENAME );

    for(i = 1 ; i < argc ; i++ ){
		if( argv[i] != NULL ){
			if( argv[i][0] == '/' ){
				strcpy( settingfilename , argv[i] );
				configfile_specified = ON;
			}
			if( argv[i][0] == '-'){
				if( argv[i][1] == 'd' && strcmp( &argv[i][1] , "d") == 0 ){
					daemon_mode = ON;
					logger("\ndaemon mode.\n");
				} else if( argv[i][1] == 'a' && strcmp( &argv[i][1] , "autoboot" )==0 ){
					auto_boot = ON;
					logger("\nautomatic reboot was set.\n");
				} else {
					char msg[100];
					sprintf(msg, "bad option: %s\n" ,argv[i] );
					logger(msg );
					return -1;
				}
			}
		} else {
			break;

		}
    }

	if( configfile_specified ==OFF ){
		logger("configuration file is not specified. use general.cf in current directory .\n");
	}
    return 0;
}

void daemon_start( int ignsigcld )
{
    int childpid;

    if( getppid() == 1 ) goto out;

#ifdef SIGTTOU
    signal( SIGTTOU , SIG_IGN );
#endif
#ifdef SIGTTIN
    signal( SIGTTIN , SIG_IGN );
#endif
#ifdef SIGTSTP
    signal( SIGTSTP , SIG_IGN );
#endif
	
    if( ( childpid = fork()) < 0 ){
		logger("fork error.\n" );
		exit(0);
    }
    else if ( childpid > 0 )
    exit(0);


	
#ifdef SIGTSTP	/* BSD */
	
#if SOLARIS25
    if( setpgrp( ) == -1 ){ 
#else
	if( setpgrp( 0 , getpid()) == -1 ){	
#endif
		
		logger("can't set process group.\n" );
		exit(0);
    }

	/* System V$B$G$O!"(Bsetpgrp()$B$9$k;~$K!"8F$S$@$7$?J}$N%W%m%;%9$,%W%m%;%9%0%k!<%W%j!<%@!<$G$J$$(B
	   $B$J$i$P!"I{:nMQE*$KC<Kv$rJ|4~$9$k$h$&$K$J$k$N$G!"0J2<$N%3!<%I$,$$$i$J$/$J$k$N$@!#(B
	   Solaris 2.5$B$G$O!"(Bsetpgrp$B$9$k;~$K$=$NI{:nMQ$,H/@8$9$k$O$:$J$N$@!#(B
	   $B$k$N$@!#(B */
	   
#if !SOLARIS25
    if( (fd = open("/dev/tty" , O_RDWR)) >= 0 ){
		ioctl( fd , TIOCNOTTY , ( char *) NULL );
		close(fd );
    }
#endif
	
#endif
	
	
	
    out:
	
    /* close file */
	
    {
		int i;
		for(i = 0 ; i < NOFILE ; i++ )
		close( i );
    }
	
    chdir( "/");

    /* reset file access mask */
    umask(0);
	
    if( ignsigcld ){
#ifdef SIGTSTP

#if !SOLARIS25
		int sig_child(void);
#else
		void sig_child(int);
#endif
		
		signal( SIGCLD , sig_child );
#endif
    }
	
	
}



void sig_child(int in)
{
#ifdef BSD
    int pid;
    union wait status;

    while( ( pid = wait3( &status , WNOHANG ,( struct rusage * ) 0 )) > 0 );
#endif
}




/********************

  $B%3%M%/%7%g%s$,$G$-$?$"$H$N:G=i$N?t%P%$%H$K$h$C$F!"$I$N%b!<%I$G%5!<%P!<$rF0$+$9$N$+$r7h$a$k!#(B
  
  HTTP$B$NMW5a$O@\B3$r$9$0@Z$k$,!"$=$l$O(Bhttp$B%k!<%A%s$NJ}$G$d$k!#(B
  $B%2!<%`$N@\B3$O(BSO_LINGER$B$9$kI,MW$,$"$k!#(B
  
  return value .... 0 .. ok
  <0 .. error
  
  ********************/
int detectMode( int sockfd )
{
#define TMPBUFSIZ 10000
    char tmpbuf[TMPBUFSIZ];
    int i;
    int len;

    len = getFReadBufLength( sockfd );
    if( len >= TMPBUFSIZ ) len = TMPBUFSIZ;
    for(i = 0; i < len ;i++){
		tmpbuf[i] = getFReadBufByte( sockfd , i );
    }
    tmpbuf[len ] = '\0';

    if( strncmp( tmpbuf , "GET" , strlen( "GET"))==0 ){
		/* HTTP$B$N(BGET */
		int ret,hret;
		ret = http_get_check(sockfd);

		if( ret ){
			connectmode[sockfd] = CONNECTION_HTTP;
			setFCloseFlg(sockfd,ON);
			hret = http_get( sockfd ,tmpbuf );
			return hret;
		} 

    }

    if( strncmp( tmpbuf , "POST" , strlen("POST"))==0){
		/* POST$B$O2?$b$7$J$$!#(B */
		connectmode[sockfd] = CONNECTION_HTTP;
		clearFBuf( sockfd );
		setFCloseFlg( sockfd , ON );
		return -1;
    }

    if( strncmp( tmpbuf , clipasswd , strlen( clipasswd )) == 0 ){
		char passwdbuf[1000];
		
		connectmode[sockfd] = CONNECTION_GAME;

		/* $B2~9T%3!<%I!"=*C<J8;z$^$GF~$l$k$N$G0lJ8;zA}$($k!#(B */
		readDataFromFBuf( sockfd , strlen( clipasswd ) , passwdbuf );
		
		return 0;
    }
    if( strncmp( tmpbuf , admpasswd , strlen( admpasswd )) == 0 ){
		char passwdbuf[1000];
		connectmode[sockfd] = CONNECTION_ADM;
		logger("adm connection");	

		readDataFromFBuf( sockfd , strlen( admpasswd ) , passwdbuf );

		strcpy( passwdbuf , "#ENTER ADM:0 ok.( use 'HELP' to display help.)\n" );
		appendFWriteBuf( sockfd , strlen( passwdbuf ) , passwdbuf );
						
		return 0 ;
    }

	
    return -1;
}

