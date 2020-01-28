#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
int main(int argc, char **argv) {
    if(argc<2) { printf("need port\n"); return 1; }
    int port = atoi(argv[1]);
        
    struct sockaddr_in si_me, si_other;

    int s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert(s>0);

    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr =INADDR_ANY;
    int r;
    r=bind(s, (struct sockaddr*)(&si_me), sizeof(si_me));
    assert(r==0);

    while (1) {
        char buf[512];
        socklen_t slen=sizeof(si_other);
        r=recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)(&si_other), &slen);
        assert(r>=0);
        printf("Echoing packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        r=sendto(s, buf, r, 0, (struct sockaddr*)(&si_other), slen);
        assert(r>=0);
    }
    return 0;
}
