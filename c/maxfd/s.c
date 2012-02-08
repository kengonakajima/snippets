#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>


int active[65536];


int
main(int argc, char ** argv )
{
    int s;
    struct sockaddr_in fuck;

    s = socket(AF_INET , SOCK_STREAM,0);

    memset(&fuck,0,sizeof(fuck));
    fuck.sin_addr.s_addr = INADDR_ANY;
    fuck.sin_port=htons(atoi( argv[1]) );
    fuck.sin_family = AF_INET;
    if( bind( s,(struct sockaddr*)&fuck,sizeof(fuck))<0){
        printf( "bind error\n" );
        return 1;
    }

    listen(s,5);
    while(1){
        int newsockfd;
        fd_set fds;
        int i, maxactive;
        struct timeval t;

        FD_ZERO(&fds);
        FD_SET( s, &fds );

        t.tv_usec = t.tv_sec = 0;
        select( s+1,&fds,(fd_set*)NULL,(fd_set*)NULL,&t );
        if( FD_ISSET(s,&fds) ){
            struct sockaddr_in cliaddr;
            int clilen = sizeof( cliaddr );
        
            newsockfd = accept( s, (struct sockaddr*)&cliaddr, &clilen );
            if( newsockfd >= 0 ){ 
                fprintf( stderr,"new socket %d\n", newsockfd );
            } else {
                fprintf( stderr,"accept fuck:%s\n", strerror(errno));
            }
            active[newsockfd]=1;
        }
        FD_ZERO( &fds );
        for(i=0;i<65536;i++ ){
            if( active[i] ){
                FD_SET( i, &fds );
                maxactive=i;
            }
        }
        t.tv_sec = t.tv_usec = 0;
        select( maxactive+1, &fds,(fd_set*)NULL,(fd_set*)NULL, &t );

        for(i=0;i<65536;i++){
            if( active[i] && FD_ISSET( i, &fds ) ){
                int r;
                char buf[100];
                r = read( i,  buf, sizeof( buf ));
                if( r<=0 ){
                    fprintf( stderr,"read error close %d\n", i );
                    active[i]=0;
                    close(i);
                } else {
                    fprintf(stderr,"R%d ", i );
                }
            }
        }
    }
}
        





    
    
