
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

void err_sys(char*hoge);
int peepservfd ,newpeepservfd;
int noticeservfd, newnoticeservfd;
int outerservfd,newouterservfd;

int main(int argc, char **argv )
{

	struct sockaddr_in cli;
	int len;
	char noticemessage[]= "please connect me\n";

	if( argc != 4 ){
		fprintf( stderr,"Usage: pserv noticeport peepport outerport\n" );
		return 1;
	}
					   
	noticeservfd = createNewAcceptableServerSocket( atoi(argv[1]) );
	if( noticeservfd <0) err_sys("notice server socket fuck"); else fprintf(stderr,"OK");
	peepservfd = createNewAcceptableServerSocket( atoi(argv[2]) );
	if( peepservfd <0) err_sys( "peep server socket fuck") ;else fprintf(stderr,"OK2");
	outerservfd = createNewAcceptableServerSocket(  atoi(argv[3]) );
	if( outerservfd <0)err_sys( "outerservfd fuck" );else fprintf(stderr,"OK3");


	newnoticeservfd = accept( noticeservfd,(struct sockaddr*)&cli,&len );
	if( newnoticeservfd < 0)err_sys("fuck accept notice");
	fprintf(stderr,"OK5");

	fprintf( stderr,"all(3) servers are ready\n");
	
	while(1){
		/* wait for new connection from outer world */
		newouterservfd = accept( outerservfd , (struct sockaddr*)&cli,&len );
		if( newouterservfd<0)break;

		/* if new connection from outer world has come,
		   write some message into notice socket and wait for new
		   connectino from inside */
		write( newnoticeservfd , noticemessage, strlen( noticemessage)+1 );

		newpeepservfd = accept( peepservfd , (struct sockaddr*)&cli ,&len );

		fprintf( stderr,"start peeping\n" );
		/* and then pass whole data from outer to peephole */
		while(1){
			fd_set rfds;
			char buf[16];	/* very small buffer on purpose not to keep write/read unseparated*/
			
			FD_ZERO( &rfds );
			FD_SET( newpeepservfd , &rfds );
			FD_SET( newouterservfd , &rfds );
			select(256 , &rfds , (fd_set*)NULL,(fd_set*)NULL,0);

			
			if( FD_ISSET( newpeepservfd , &rfds )){
				/* data came from inside */
				int nr;
				nr = read( newpeepservfd , buf , sizeof( buf ) );
				if( nr <= 0 )break;
				write( newouterservfd , buf , nr );
			}

			if( FD_ISSET( newouterservfd, &rfds ) ){
				int nr;
				nr = read( newouterservfd , buf , sizeof( buf ) );
				if( nr <= 0) break;
				write( newpeepservfd , buf , nr );
			}
		}
		
		/* finally close outer socket and peep hole too*/
		close( newouterservfd );
		close( newpeepservfd );
		
	}

	/* close all sockets alive */
	
	close( newnoticeservfd );
	close( noticeservfd );
	close( outerservfd );
	close( peepservfd );
	
	
}

/*
   return a fd which is ready to accept new connection

*/   
int createNewAcceptableServerSocket( int port )
{
	int fd= socket( AF_INET , SOCK_STREAM , 0 );
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons( ((unsigned short)port) );
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind( fd ,(struct sockaddr*)&sin,sizeof(struct sockaddr))<0){
		return -1;
	}

	listen(fd,5 );
/*	signal(SIGPIPE , SIG_IGN );*/

	return fd;
}


void err_sys(char*hoge)
{
	extern int errno;
	fprintf( stderr, "%s errno:%d\n" ,hoge,errno );
	exit(1);
}

