#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
/*
  	
	クライアントの方のプログラム

*/

#define perror(xxx) ;


struct sockaddr_in sockadr,peer;
int peeraddrlen;
int sockfd,newsockfd;

int port = 6000;

struct hostent *ht;
char hostname[20];

int datalen = 32;

extern int errno;
extern int h_errno;
unsigned int  pr_inet(char **listptr,int length);

int main( int argc , char **argv)
{
	int ret;
	int i;
	int counter;
	
	unsigned char *buf;
	
	if(argc != 4) {
		printf("usage: a.out <hostname> <port no> <datalen>\n");
		exit(0);
	}
	
	port = atoi(argv[2]);
	datalen = atoi(argv[3] );
	if(datalen == 0) {
		fprintf(stderr,"Data length error.\n");
		exit(-1);
	} else {
		buf = (char *) malloc( datalen );
	}

	strcpy(hostname,argv[1]);
	
	sockfd = socket(AF_INET,SOCK_STREAM,0);	/* ほとんどは0にするって */

	ht = gethostbyname(hostname);
	if(ht == NULL){
		perror("Oh! no. gethost failed.\n");
		fprintf(stderr,"%d",h_errno);
		exit(-1);
	}

	sockadr.sin_family = AF_INET;
	sockadr.sin_port = port ;
	sockadr.sin_addr.s_addr  =  pr_inet(ht->h_addr_list,ht->h_length) ;

	ret = connect(sockfd, (struct sockaddr *) &sockadr,sizeof(struct sockaddr_in) );
	if( ret < 0){
		fprintf(stderr,"connect error. \n");
		perror( (char *)strerror(errno));
		exit(-1);
	}

	for(counter = 0;;counter ++){

		buf[0] = 'a';
		buf[1] = '\0';
		
		write(sockfd,buf,datalen -1);
		read(sockfd,buf,datalen -1 );

		if( ( counter & 1023) == 0) {
			fprintf(stderr,"!");
		}

	}
	
	close(sockfd);
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

