#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> 
#include <fcntl.h>

 
int main(int argc, char* argv[]) {
    if(argc!=2) {
        printf("need server ip\n");
        return 1;
    }

    const int N=300;
    int sp=10000;
    int fds[N];    
    for(int i=0;i<N;i++) {
        fds[i]=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        assert(fds[i]>0);
        struct sockaddr_in myaddr;
        memset((char *) &myaddr, 0, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_port = htons(sp+i);
        myaddr.sin_addr.s_addr =INADDR_ANY;

        int r;
        r=bind(fds[i], (struct sockaddr*)(&myaddr), sizeof(myaddr));
        assert(r==0);
        
        struct sockaddr_in saddr;
        int slen=sizeof(saddr);
        memset((char *) &saddr, 0, sizeof(saddr));
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(sp+i);
        r=inet_aton(argv[1], &saddr.sin_addr);
        assert(r==1);
        
        r=sendto(fds[i], "hi", 2, 0, (struct sockaddr*)(&saddr), slen);
        assert(r>=0);
    }
    return 0;
}
