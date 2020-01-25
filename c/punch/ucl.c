#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h> 

int main(int argc, char* argv[]) {
    if(argc!=2) {
        printf("need server ip\n");
        return 1;
    }
    struct sockaddr_in si_me, si_sv;
    int s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert(s>0);

    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(10000); // any port
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
 
    memset((char *) &si_sv, 0, sizeof(si_sv));
    si_sv.sin_family = AF_INET;
    si_sv.sin_port = htons(10000);
    int r=inet_aton(argv[1], &si_sv.sin_addr);
    assert(r==1);

    socklen_t slen=sizeof(si_sv);
    r=sendto(s, "hi", 2, 0, (struct sockaddr*)(&si_sv), slen);
    assert(r>=0);

    char buf[100];
    struct sockaddr_in si_other;
    slen=sizeof(si_other);
    r=recvfrom(s, &buf, sizeof(buf), 0, (struct sockaddr*)(&si_other), &slen);
    assert(r>=0);
    printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        
    return 0;
}
