#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_UDP_PORT 8000
#define SERV_TCP_PORT 8000

#define GOBOU "192.50.220.81"
#define TETO  "192.50.220.67"
#define MIKE  "192.50.220.35"
#define NEGI  "192.50.220.71"

char hosts[10][20] = {
    GOBOU , TETO , MIKE , NEGI
};
char *pname;

struct gene{
    int sockfd , newsockfd;
}gene;

void printerr( char msg[] );

void main(int argc , char **argv )
{
	pname = argv[0];
	
	if( ( gene.sockfd = socket( AF_INET , SOCK_STREAM , 0 )) < 0 ){
	    printerr(" server: ソケットを開けない。 \n");
	    exit(1);

	}

}

void printerr( char msg[] )
{
    fprintf(stderr,msg );
}

	
