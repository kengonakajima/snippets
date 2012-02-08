/*









  fdopen test for sockets
  






 */


#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>
#include <sys/time.h>


int main( int argc ,char **argv )
{
    int s ,ns;
    struct sockaddr_in sin;
    struct sockaddr_in cli;
    int clilen;
    
    s = socket( AF_INET , SOCK_STREAM , 0 );

    bzero( &sin ,sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port =htons(7001);
    sin.sin_addr.s_addr=htonl( INADDR_ANY);

    if(bind( s , (struct sockaddr *) &sin , sizeof(sin))<0){
        fprintf(stderr ,"bind error\n");
        exit(1);
    }
    
    listen(s , 5 );

    clilen= sizeof( cli);
    ns = accept( s , (struct sockaddr*)&cli , &clilen);
    if( ns < 0 ){
        fprintf(stderr , "accept error\n");
        exit(1);
    }
    fprintf(stderr , "NS:%d\n" , ns );

    /* fdopen here! */


    {
        FILE *fp;
        char line[100];
        
        fp = fdopen( ns , "r+");
        while(fgets( line ,sizeof(line) , fp )){
            fprintf(stderr , "Fucking line:[%s]\n", line );
            fprintf( fp , "LINE:%s\n", line);
        }
    }
    
    fprintf(stderr , "close!!");
    
    close(s);
    close(ns);
}
