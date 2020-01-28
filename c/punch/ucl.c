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

int32_t set_socket_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    assert(flags >= 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}



int main(int argc, char* argv[]) {
    if(argc!=4) {
        printf("args: localPort destIP destPort\n");
        return 1;
    }
    int localPort=atoi(argv[1]);
    char *destIP=argv[2];
    int destPort=atoi(argv[3]);
    
    struct sockaddr_in si_me, si_sv;
    int s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert(s>0);
    set_socket_nonblock(s);

    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(localPort); // any port
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    int r=bind(s, (struct sockaddr*)(&si_me), sizeof(si_me));
    assert(r==0);
    
 
    memset((char *) &si_sv, 0, sizeof(si_sv));
    si_sv.sin_family = AF_INET;
    si_sv.sin_port = htons(destPort);
    r=inet_aton(destIP, &si_sv.sin_addr);
    assert(r==1);

    int cnt=0;
    while(1) {
        cnt++;

        if((cnt%1000)==0) {
            socklen_t slen=sizeof(si_sv);
            r=sendto(s, "hi", 2, 0, (struct sockaddr*)(&si_sv), slen);
            assert(r>=0);
        }

        char buf[100];
        struct sockaddr_in si_other;
        socklen_t slen=sizeof(si_other);
        r=recvfrom(s, &buf, sizeof(buf), 0, (struct sockaddr*)(&si_other), &slen);
        if(r>0) {
            printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        }
        usleep(1000);
    }        
    return 0;
}
