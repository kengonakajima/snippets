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
    int s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert(s>0);


    const int N=300;
    int sp=10000;
    for(int i=0;i<N;i++) {
    struct sockaddr_in saddr;
        int slen=sizeof(saddr);
        memset((char *) &saddr, 0, sizeof(saddr));
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(sp+i);
        int r=inet_aton(argv[1], &saddr.sin_addr);
        assert(r==1);
        
        r=sendto(s, "hi", 2, 0, (struct sockaddr*)(&saddr), slen);
        assert(r>=0);
    }
    return 0;
}
