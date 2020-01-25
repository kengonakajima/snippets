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
    si_me.sin_port = htons(0); // any port
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
 
    memset((char *) &si_sv, 0, sizeof(si_sv));
    si_sv.sin_family = AF_INET;
    si_sv.sin_port = htons(9999);
    int r=inet_aton(argv[1], &si_sv.sin_addr);
    assert(r==1);

    struct client server;    
    server.host = si_sv.sin_addr.s_addr;
    server.port = si_sv.sin_port;
 
    // hello to server
    int slen=sizeof(si_sv);
    r=sendto(s, "hi", 2, 0, (struct sockaddr*)(&si_sv), slen);
    assert(r>=0);

    // expect NAT keep host/port in its session store
    int ncli=0;
    struct client peers[10]; 
    while (1) {
        struct client buf;
        struct sockaddr_in si_other;
        socklen_t slen=sizeof(si_other);
        r=recvfrom(s, &buf, sizeof(buf), 0, (struct sockaddr*)(&si_other), &slen);
        assert(r>=0);
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        
        if (server.host == si_other.sin_addr.s_addr && server.port == (short)(si_other.sin_port)) {
            printf("from server!\n");
            int found=0;
            // Now we just have to add the reported peer into our peer list
            for(int i = 0; i < ncli; i++) {
                if (peers[i].host == buf.host && peers[i].port == buf.port) {
                    found = 1;
                    break;
                }
            }
            // Only add it if we didn't have it before.
            if(!found) {
                peers[ncli].host = buf.host;
                peers[ncli].port = buf.port;
                ncli++;
            }
            si_other.sin_addr.s_addr = buf.host;
            si_other.sin_port = buf.port;
            printf("Added peer %s:%d, ncli:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), ncli );
            for(int k = 0; k < 10; k++) {
                for(int i = 0; i < ncli; i++) {
                    si_other.sin_addr.s_addr = peers[i].host;
                    si_other.sin_port = peers[i].port;
                    printf("sending hi to peer %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port) );
                    r=sendto(s, "hi", 2, 0, (struct sockaddr*)(&si_other), slen);
                    assert(r>=0);
                }
            }
        } else{
            printf("from other client!\n");
            for(int i = 0; i < ncli; i++) {
                if (peers[i].host == buf.host && peers[i].port == (short)(buf.port)) {
                    printf("Received from peer %d!\n", i);
                    break;
                }
            }
        }
    }
    close(s);
    return 0;
}
