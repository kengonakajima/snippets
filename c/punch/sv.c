#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct client {
    int host;
    short port;
};
 
int main(void) {
    struct sockaddr_in si_me, si_other;
    struct client clients[10]; 

    int s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert(s>0);

    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(9999);
    si_me.sin_addr.s_addr =INADDR_ANY;
    int r;
    r=bind(s, (struct sockaddr*)(&si_me), sizeof(si_me));
    assert(r==0);

    int n=0;
    while (1) {
        char buf[512];
        socklen_t slen=sizeof(si_other);
        r=recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr*)(&si_other), &slen);
        assert(r>=0);
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        clients[n].host = si_other.sin_addr.s_addr;
        clients[n].port = si_other.sin_port;
        n++;
        for (int i = 0; i < n; i++) {
            si_other.sin_addr.s_addr = clients[i].host;
            si_other.sin_port = clients[i].port;
            for (int j = 0; j < n; j++) {
                printf("Sending to %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
                r=sendto(s, &clients[j], 6, 0, (struct sockaddr*)(&si_other), slen);
                assert(r>=0);
            }
        }
        printf("Now we have %d clients\n", n);
    }
    close(s);
    return 0;
}
