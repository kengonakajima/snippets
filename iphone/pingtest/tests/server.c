#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

/*
  ���bind����ȡ��⤦���Υ��ɥ쥹�ϥץ�����λ���Ƥ�Ȥ��ʤ��ʤäƤ��롣
*/



struct sockaddr_in sockadr,peer;
socklen_t peeraddrlen;
int sockfd,newsockfd;

int port = 6000;

struct hostent *ht;
char hostname[20];

int datalen = 32;

extern int errno;
extern int h_errno;
unsigned int  pr_inet(char **listptr,int length);

int playerno = 1;

int main( int argc , char **argv)
{
	int ret;
	unsigned char *buf;
	int i;
	int counter = 0;

    if( argc != 3 ){
        fprintf(stderr, "usage: a.out host port\n");
        return 1;
    }
    
	sockfd = socket(AF_INET,SOCK_STREAM,0);	/* �ۤȤ�ɤ�0�ˤ���ä� */
	

	ht = gethostbyname( argv[1]);
	if(ht == NULL){
		fprintf(stderr, "Oh! no. gethost failed.\n");
		fprintf(stderr,"%d",h_errno);
        return 1;
	}
	port = atoi( argv[2] );
	
	sockadr.sin_family = AF_INET;
	sockadr.sin_port = htons(port);
	sockadr.sin_addr.s_addr  =  pr_inet(ht->h_addr_list,ht->h_length) ;
		
	ret = bind(sockfd,(struct sockaddr *) &sockadr,sizeof(struct sockaddr_in) );
	if( ret < 0){
		fprintf(stderr,"bind error.errno%d\n",errno);
		fprintf(stderr, (char *)strerror(errno));
        return 1;
	}

    ret = listen(sockfd,5);
    if(ret < 0){
        fprintf(stderr,"listen error.\n");
        fprintf(stderr, (char *) strerror(errno) );
        return 1;
    } 

        
    while(1){
        fprintf(stderr, "endless loop.\n");

        //

        newsockfd = accept(sockfd, (struct sockaddr *)&peer , &peeraddrlen);
        if( newsockfd < 0){
            fprintf(stderr,"accept error.\n");
            fprintf(stderr,(char *) strerror(errno) );
            return 1;
        }
        fprintf(stderr, "accepted\n");
            

        //	buf = "HTTP/1.0 200\nContent-Type: text/plain\n\n";
        //	write( newsockfd , buf , strlen(buf));
	
        while(1)
            {
                int i;

                if(read( newsockfd , &i , 1 )<= 0 )break;
                write( 1 , &i , 1 );
                write( newsockfd, &i, 1);
		
            }
        close( newsockfd );
    }

    return 0;
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
