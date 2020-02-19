#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    if(argc!=4) {
        printf("arg: local_port server_ip server_port\n");
        return 1;
    }


    struct sockaddr_in origsi;
    memset((char *) &origsi, 0, sizeof(origsi));
    origsi.sin_family = AF_INET;
    int r=inet_aton("0.0.0.0", &origsi.sin_addr);
    assert(r==1);
    origsi.sin_port = htons(atoi(argv[1]));

    
    struct sockaddr_in destsi; 
    memset((char *) &destsi, 0, sizeof(destsi));
    destsi.sin_family = AF_INET;
    r=inet_aton(argv[2], &destsi.sin_addr);
    assert(r==1);
    destsi.sin_port = htons(atoi(argv[3]));

    int s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert(s>0);
    r=bind(s,(struct sockaddr*)&origsi,sizeof(origsi));
    if(r<0) {
        fprintf(stderr,"bind error: %s\n",strerror(errno));
        return 1;
    }
    
    // hello to server
    r=sendto(s, "hi", 2, 0, (struct sockaddr*)(&destsi), sizeof(destsi));
    if(r<0) {
        fprintf(stderr,"sendto error: %s\n",strerror(errno));
        return 1;
    }

    char buf[10];
    struct sockaddr_in recvsi;
    socklen_t slen=sizeof(recvsi);
    r=recvfrom(s, &buf, sizeof(buf), 0, (struct sockaddr*)(&recvsi), &slen);
    printf("recvfrom:%d from %s:%d\n", r, inet_ntoa(recvsi.sin_addr), ntohs(recvsi.sin_port));
    
    close(s);
    return 0;
}


