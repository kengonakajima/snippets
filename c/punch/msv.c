#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int32_t set_socket_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    assert(flags >= 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(void) {
    const int N=300;
    int sp=10000;
    int fds[N];
    int r;
    for(int i=0;i<N;i++){
        fds[i]=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        assert(fds[i]>0);
        r=set_socket_nonblock(fds[i]);
        assert(r>=0);
        struct sockaddr_in saddr;
        saddr.sin_family=AF_INET;
        saddr.sin_port=htons(sp+i);
        saddr.sin_addr.s_addr = INADDR_ANY;
        r=bind(fds[i],(struct sockaddr*)&saddr,sizeof(saddr));
        assert(r>=0);
    }
    printf("sockets prepared\n");

    while (1) {
        for(int i=0;i<N;i++) {
            char buf[512];
            struct sockaddr_in peeraddr;
            socklen_t slen=sizeof(peeraddr);
            r=recvfrom(fds[i],buf,sizeof(buf),0, (struct sockaddr*)&peeraddr,&slen);
            if(r>0) {
                printf("Received packet from %s:%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
                r=sendto(fds[i],buf,sizeof(buf),0,(struct sockaddr*)&peeraddr,slen);
                if(r<0) printf("send error\n");                
            }
        }
        usleep(1000);
        static int cnt=0;
        cnt++;
        if((cnt%100)==0) fprintf(stderr,".");
    }
    return 0;
}
