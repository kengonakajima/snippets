#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

struct sockaddr_in sockadr,cliadr;
int sockfd,newsockfd,clilen;


/*
   受信用のsockfdを共通に持っていて、それぞれが同じローカルアドレスで待つ。
*/

int childno = 0;		/* 子プロセスの数 */

int pidtable[500];
int childflg = 0;

void cleanChild(void )
{
	int p;
	int ret;
	int k;
	
	for(;; ){

		p = waitpid(-1 , &ret, WNOHANG );
		
		if( p <= 0 )
			break;
		fprintf(stderr,"CLEAN:pid%d\n" , p );
	
	}
	
}


int main( int argc , char **argv)
{
	int ret;
	int optflg = 1;
	int i;

	sockfd = socket(AF_INET,SOCK_STREAM,0);	/* ほとんどは0にするって */

	bzero( (char *) &sockadr  ,sizeof( sockadr ));
	sockadr.sin_family = AF_INET;
	sockadr.sin_port = htons( atoi( argv[1] ));
	sockadr.sin_addr.s_addr  =  htonl( INADDR_ANY );

/* ここでオプション設定 */
	setsockopt(  sockfd , SOL_SOCKET , SO_REUSEADDR , &optflg , sizeof(int));
	
	ret = bind(sockfd,(struct sockaddr *) &sockadr ,sizeof(struct sockaddr_in) );
	if( ret < 0){
        extern int errno;
		fprintf(stderr,"bind error.:%s",strerror(errno));
		exit(-1);
	}

	

	/* 必要な個数forkする */

	for(i = 0 ; i < 4 ; i++ ){
		int p;
		if( (p = fork() ) == 0 ){
			/* I am a child */
			childflg = 1;
			
			break;
		} else {
			/* I am parent */
			pidtable[i] = p;
			fprintf(stderr,"Child(%d) pid=%d\n" , i , p );
		}
	}

	if( childflg == 0 ){
		/* 親は業務を行なわない。 */
		while(1){
			sleep(1);
			cleanChild();
			fprintf(stderr,".");
		}
		
	} else {
		listen(sockfd,5);		

		while(1){
			newsockfd = accept(sockfd, (struct sockaddr *)&cliadr , &clilen);
			if( newsockfd < 0 ){
				fprintf(stderr,"accept error.\n");
				exit(-1);
			}		
			while( 1){
				char buf[1000];
				bzero( buf , 1000 );
				if( read( newsockfd, buf , 1 ) < 0 ) break;
				if( write( newsockfd , buf , 1 ) < 0 ) break;
				
				/*fputs( buf ,stderr); */
				fprintf( stderr,"%s" , buf );
	/*			fprintf( stderr,"(%d)" , getpid() );*/
			}
			close( newsockfd );
		
		}
		close( sockfd );
	}
}





