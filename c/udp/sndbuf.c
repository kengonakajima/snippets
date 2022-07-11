
#include <stdio.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main() {
    struct sockaddr_in ia;
    int sk=socket(PF_INET,SOCK_DGRAM,0);
    printf("socket:%d\n",sk);


    int r;
    int val;
    socklen_t len=sizeof(val);
    r=getsockopt(sk, SOL_SOCKET, SO_SNDBUF, &val, &len);
    printf("ret:%d val:%d l:%d\n",r,val,len);
    return 0;
}
