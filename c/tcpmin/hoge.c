#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
int main() {
    int fd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in sa;
    sa.sin_family=AF_INET;
    sa.sin_port=htons(80);
    sa.sin_addr.s_addr=INADDR_ANY;
    int re=bind(fd,(struct sockaddr*)&sa,sizeof(sa));
    printf("bind ret:%d (%s)\n",re, strerror(errno));
    return 0;
}
