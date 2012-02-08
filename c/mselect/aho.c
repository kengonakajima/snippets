

#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include <sys/file.h>
#include <listen.h>

#include <sys/sockio.h>
#include <sys/socketvar.h>

#include <arpa/nameser.h>
#include <resolv.h>

#include <netdb.h>


/*

	selectは、sys/select.hを見るかぎり、デフォルトで1024本までいけるみたいだが、
	その実験をする。

	実際にするサービスは、echoサーバである。
*/

#define ON 	1
#define OFF 0

unsigned int  pr_inet(char **listptr,int length);
void client(char *hostname , int port ,int maxconnection);


int main(int argc, char **argv )
{

	fd_set fdv , rfdv ,  wfdv;
	struct timeval tm;
	struct sockaddr_in cli_addr , serv_addr;
	int clilen;
	int i;
	int sockfd , newsockfd ;
	int sockfd_table[2000];
#define MAX_SOCKFD 1024
	int maxconnection = MAX_SOCKFD;
	int fdlist[MAX_SOCKFD];
	int fdread[MAX_SOCKFD];
	int fdactive[MAX_SOCKFD];
	int port;
	int fdnum;
	

	if( argc < 3 ){
		fprintf(stderr,"Usage: a.out -s|c portno host(client mode)\n-s:server -c:client\n");
		exit(0);
	}

	port = atoi( argv[2] );

	if( strcmp( argv[1] , "-c" ) ==0 ){
		/* client program */
		if( argc <4 ){
			fprintf(stderr,"Usage: a.out -s|c portno host(client mode)\n-s:server -c:client\n");
			exit(0);
		}
		client( argv[3] , port ,maxconnection-4);
		exit(9);
	}
		
	/* Make server socket */

	if( ( sockfd = socket( AF_INET , SOCK_STREAM , 0 ) ) < 0 ){
		fprintf(stderr,"can't make server socket\n" );
		exit(0);
	}
	
	for(i=0;i<maxconnection;i++){
		fdlist[ i ] = OFF;
		fdread[ i ] = 0;
		fdactive[i] = OFF;
	}
	
	fdlist[ sockfd ] = ON;
	
	bzero( (char *) &serv_addr , sizeof( serv_addr ));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons( port );
	serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );

	if( bind ( sockfd , (struct sockaddr *) &serv_addr , sizeof( struct sockaddr )) < 0 ){
		fprintf( stderr , "bind error.\n");
		exit(9);
	}

	listen( sockfd , 5 );
	fprintf( stderr,"start loop.\n");

	fprintf(stderr,"server socket fd= %d\n" , sockfd );
	
	for(;; ){
		for(;;){
			
			
			fdlist[sockfd] = ON;
			tm.tv_sec = 0;
			tm.tv_usec = 50000; /* 50 ms */

			FD_ZERO( &fdv );
			FD_ZERO( &rfdv );
			FD_ZERO( &wfdv );

			for(i=0 , fdnum = 0; i < maxconnection ; i++ ){
				if( fdlist[i] == ON ){
					FD_SET( i , &fdv );
					FD_SET( i , &rfdv );
					FD_SET( i , &wfdv );
					fdnum++;
				}
			}

			select( maxconnection  , &rfdv , &wfdv , (fd_set*) 0 , &tm );

			for(i=0; i < maxconnection ;i++ ){
				fdactive[i] = OFF;
				if( FD_ISSET( i , &rfdv )){
					if( i == sockfd ){
						/* Main socket was selected */
						clilen = sizeof( cli_addr );
						newsockfd = accept( sockfd , (struct sockaddr *) &cli_addr , &clilen);
						if( newsockfd < 0 ){
							fprintf(stderr,"accept returned minus value.\n" );
							continue;
						}
						fprintf(stderr,"accepted fd = %d\n" , newsockfd );
						fdlist[newsockfd] = ON;
						fcntl( newsockfd , F_SETFL , FNDELAY );
					} else {
						int ret;
						char tmpbuf[ 10000];
						ret = read( i , tmpbuf , 1 );	/* Read a byte */
						if( ret == 1 ){
							/* Successfully read */
							fdread[i] ++;
							fdactive[i] = ON;
							/* Do echo */
							write( i , tmpbuf , 1 );
							
						} else if( ret == 0 ){
							/* Something bad */
						} else if( ret < 0 ){
							/* Error */
							close( i );
						}
					}
				}
			}	 /* for(i=0;i<maxconnection;i++) */

			/* Print status */
			fprintf(stderr,"\nstatus:\n\n" );
			
			for(i=0;i<maxconnection;i++){
				
				/* newline */
				if( i != 0 && i % 64 == 0 ) fprintf( stderr,"\n");
				if( fdlist[i] == ON ){
					if( fdactive[i] == ON ){
						fputc( '*' , stderr );
					} else {
						fputc( '+' , stderr );
					}
				} else {
					fputc( '-' , stderr );
				}
				
			}

			usleep(200*1000);
			
		} 	/* for(;;) */
	}	/* for(;;) */
	
}

extern int errno;


void client( char *hostname , int port ,int maxconnection)
{
	struct hostent *ht;
	int clifdlist[10000];
	int i;
	int sockfd;
	struct sockaddr_in sockaddr;
	int c;
	int established =0;

	fprintf( stderr,"port: %d\n" , port );
	
	/* First , clean clifdlist, which stands for active connection flags */
	for(i=0;i<maxconnection ;i++){
		clifdlist[i] = OFF;
	}
	
	ht = gethostbyname( hostname );
	if( ht == NULL ){
		fprintf( stderr,"gethostbyname error.\n");
		exit(0);
	}




	/* Now , information for connection is completed. */
	/* First of all , make connection to server as many as it can */

	for(i=0;i<maxconnection;i++){
		int ret;
		
		sockfd = socket( AF_INET , SOCK_STREAM , 0 );
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_port = htons(port);
		sockaddr.sin_addr.s_addr = pr_inet( ht->h_addr_list , ht->h_length );
		ret = connect( sockfd,  (struct sockaddr *)&sockaddr ,sizeof(struct sockaddr_in ));
		if( ret < 0 ){
			fprintf( stderr  ,"connect error: %s\n" , strerror( errno ));
			continue;
		}
		fprintf(stderr,"established: fd:%d\n" , sockfd );
		clifdlist[sockfd] = ON;
		established++;
		usleep( 100*1000 );
	}

	fprintf( stderr,"Established %d connections. now test them.\n" , established);
	
	/* Next , send data through established connection */

	for(c=0;;c++){
		int wrote=0, readed=0;
		for(i=0;i<maxconnection;i++ ){

			if( clifdlist[i] == ON ){
				int k;
				char message[ ] = "This string will be sended to server and echoed";
				char tmpbuf[10];
				tmpbuf[0] = message[ c % strlen( message ) ];
				tmpbuf[1] = 0;
				k = write( i , tmpbuf , 1 );
				if( k > 0 ) wrote += k;
			}
		}
		fprintf( stderr, "wrote %d counts of data\n" , wrote );
									 
		/* finally , read data from established connection */

		for(i=0;i<maxconnection;i++){
			int k;
			if( clifdlist[i] == ON ){
				char readbuf[100];
				k = read( i , readbuf , 1 );
				if( k > 0 ) readed += k;
				fprintf( stderr," %d:%c" , i , readbuf[0] );
			}
		}
		fprintf( stderr , "read %d counts of data\n" , readed );
	}
}

unsigned int  pr_inet(char **listptr,int length)
{
	struct in_addr *ptr;
	unsigned long ret;
	
	while( ( ptr = (struct in_addr *) *listptr++) != NULL){
		printf("    In integer: %d\n",ret = inet_addr(inet_ntoa(*ptr)) );
		printf("    Internet address: %s\n",inet_ntoa(*ptr));
		
	}
	return ret;
}

