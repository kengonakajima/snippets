

/*
   クライアントからの接続要求に答える。

   forkされたサーバープロセスは、プレイヤの操作の頻度を管理する。
   
   
*/   
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


void doIt( int sockfd );
void sigint_hook( void );
void sigchld_hook( void );

#define PORT 9000

int main( int argc , char **argv )
{
	int sockfd , newsockfd ,clilen , childpid , servlen;
	struct sockaddr_in cli_addr , serv_addr;
	int i;
	int siz;
	

	if( argc == 1 ){
		fprintf(stderr,"usage: stest siz\n");
		exit(1);
	}

	siz = atoi( argv[1] );
	
	if( ( sockfd = socket( AF_INET , SOCK_STREAM , 0)) < 0){
		fprintf(stderr,"server: socket error.\n");
		exit(0);
	}

	
	bzero(( char *) &serv_addr , sizeof( serv_addr ));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
	serv_addr.sin_port = htons( PORT );

	if( bind( sockfd ,( struct sockaddr *) & serv_addr , sizeof(serv_addr ) ) < 0 ){
	    	fprintf(stderr,"server: bind error.\n");
		exit(1);
	}

	listen( sockfd , 5 );


    	clilen = sizeof( cli_addr );
	newsockfd = accept( sockfd , ( struct sockaddr *) & cli_addr , &clilen );	

	{

		int i;
		int data[32768];
		int n;
		
		for( ; ; ){
			n = read( newsockfd , data , siz*4 );
			fprintf(stderr ,"received data:%dbyte  data[0] %d data[siz-1] %d\n" , n ,
				data[0] , data[siz-1] );
			if( n <= 0 ){
				close( newsockfd );
				close( sockfd );
				exit(0);
			}		
		}
	}
	
	
}
