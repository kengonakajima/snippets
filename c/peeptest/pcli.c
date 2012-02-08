#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int s_pipe( int fd[2] );
int exec_telnet( int fd ,char**,char**);
void err_sys(char*hoge);
int createNewClientSocket( char *hname ,int port );

int noticefd;
int peepfd;

int main( int argc , char **argv, char**envp )
{
	int fd[2] , childpid;

	
/*	if( !isatty(0) || !isatty(1)){err_sys("fucktty.");}*/

	if(argc != 4 ) err_sys("fuckargs. a.out peeperhostname noticeport peepport\n");

	noticefd = createNewClientSocket( argv[1] , atoi(argv[2]));
	printf( "created notice fd:%d\n" , noticefd );
	
	while(1){
		char buf[512];	/* for notice */
		int nr;
		nr = read( noticefd, buf , sizeof(buf ) );
		if( nr == 0 )continue; else if( nr < 0 )break;
		printf( "notice!\n");
		peepfd = createNewClientSocket( "ns" , atoi(argv[3]) );
		if( peepfd< 0 ){
			printf( "peep socket fucked\n");
			continue;
		}

		if( s_pipe(fd)<0){err_sys( "fucks_pipe.");}

		if( ( childpid = fork())<0){err_sys( "cant fork");}

		if( childpid == 0 ){
			close(fd[0]);
			exec_telnet( fd[1],argv,envp);
		} 
		close(fd[1] );
		pass_all( fd[0] , childpid );

		close(peepfd);
	}
	close(noticefd );
	
	exit(0);

}

/*
   return fdof peep socket
*/
int createNewClientSocket( char *hname, int port )
{
	int fd = socket( AF_INET , SOCK_STREAM , 0 );
	struct hostent *h;
	struct in_addr ina;
	struct sockaddr_in sin;

	if( fd <0 )err_sys( "socket error");
	h = gethostbyname( hname );
	if( h == NULL )err_sys( "gethostname error");

	bcopy( h->h_addr_list[0] , &ina.s_addr , 4 );
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port );
	sin.sin_addr = ina;

	if( connect( fd , (struct sockaddr*)&sin , sizeof( struct sockaddr_in ) ) < 0 ) err_sys("connect error");

	return fd;
}


int pass_all( int fd  )
{
	int maxfdp1 , nfound , nread;
	char buff[512];
	fd_set readmask;

	FD_ZERO( &readmask );

#if 0
	{
		char txt[]="open lsdev\nroot\nrootpass\n";
		write(fd,txt,strlen(txt));
	}
#endif
	
	for(;;){
		FD_SET( peepfd , &readmask );
		FD_SET( fd, &readmask );
		maxfdp1 = fd + 1;
		nfound = select( maxfdp1 , &readmask , (fd_set*)0,(fd_set*)0,(struct timeval*)0 );
		if( nfound < 0 ){printf("selectfuck\n");}
		if( FD_ISSET(peepfd,&readmask)){
			nread = read(peepfd,buff,sizeof(buff));
			if( nread<0){err_sys("read0fuck");}
			if( nread==0)break;
			if( write( fd , buff, nread ) != nread ){printf("fuck\n");}
		}
		if( FD_ISSET( fd , &readmask ) ){
			nread = read( fd , buff , sizeof( buff ) );
			if( nread <= 0 ) break;
			if( write(1,buff , nread)!= nread ){
				printf("writestdouterr\n");
			}
		}
	}
}

int exec_telnet( int fd ,char**argv,char**envp)
{
	char telnet[] = "/bin/telnet";

	close(0);close(1);close(2);
		
	if( dup(fd) != 0 || dup(fd) != 1 || dup(fd) != 2 )err_sys( "dup error");

	close(fd);


	execve( telnet ,argv,envp);
	err_sys("execfuck\n");
	return -1;
}

int s_pipe( int fd[2] )
{
	return ( socketpair( AF_UNIX , SOCK_STREAM , 0 , fd ) );
}



void err_sys(char*hoge)
{
	extern int errno;
	fprintf( stderr, "%s errno:%d\n" ,hoge,errno );
	exit(1);
}





