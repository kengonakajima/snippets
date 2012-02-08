#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


int active[65536];


int
main(int argc, char ** argv )
{
    int s,i;
    struct sockaddr_in fuck;


    if( argc != 4 ){
        fprintf(stderr,"Usage: c port number bindstart\n" );
        return 0;
    }
    
    for(i=0;i<atoi(argv[2]);i++){
        int ccc;
        struct sockaddr_in bindaddr;


        usleep(70*1000);
        s = socket(AF_INET , SOCK_STREAM,0);

        memset( &bindaddr,0,sizeof(bindaddr));
        bindaddr.sin_family = AF_INET;
        bindaddr.sin_port = htons( atoi(argv[3])) + i;
        bindaddr.sin_addr.s_addr = inet_addr("192.168.1.78" );
        if( bind( s, (struct sockaddr*)&bindaddr, sizeof( bindaddr))<0){
            fprintf(stderr,"bind error : port: %d\n",
                    bindaddr.sin_port );
            return 1;
        }
            
        memset(&fuck,0,sizeof(fuck));
        fuck.sin_addr.s_addr = inet_addr( "127.0.0.1" );
        fuck.sin_port=htons(atoi(argv[1]) );
        fuck.sin_family = AF_INET;
    again:
        
        if( (ccc=connect( s,(struct sockaddr*)&fuck,sizeof(fuck)))<0){
            if( errno == EAGAIN ){
                fprintf(stderr,"EAGAIN\n" );
                goto again;
            }
            fprintf(stderr,"connect fuck: %d%s\n",errno,strerror(errno));

            break;
        } else {
            fprintf(stderr,"connect ok fd:%d\n", s );
            active[s]=1;
        }
    }

    while(1){
        int i;
        sleep(1);
        for(i=0;i<65536;i++){
            if( active[i] ){
                int r;
                char buf[10] ="aho";
                
                r = write( i, buf, 1 );
                if( r <= 0 ){
                    fprintf(stderr,"WE close\n");
                    close(i);
                    active[i]=0;
                }
            }
        }
    }

    return 0;
}
        

