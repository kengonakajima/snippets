//
//  poo.c
//  mrutest
//
//  Created by Kengo Nakajima on 2020/07/27.
//  Copyright © 2020 Kengo Nakajima. All rights reserved.
//

#include "poo.h"

#include <assert.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

struct sockaddr_in ia;

int sk=-1;
void on_start() {
    printf("on_start\n");
    sk=socket(PF_INET,SOCK_DGRAM,0);
    assert(sk>=0);
    printf("socket:%d\n",sk);
    int r = fcntl(sk, F_SETFL, O_NONBLOCK | (fcntl(sk, F_GETFL) & ~O_NONBLOCK));
    assert(r==0);

    const char *addrstr="139.162.118.45";

    if(inet_pton(AF_INET, addrstr,&ia.sin_addr.s_addr)==0) {
        fprintf(stderr,"inet_pton inet failed: %d\n",errno);
        return;
    }

    ia.sin_port=htons(23456);
    ia.sin_family=AF_INET;
    ia.sin_len=4; // Apple

    r=bind(sk,(struct sockaddr*)&ia, sizeof(ia));
    if(r<0) {
        fprintf(stderr,"bind error:%d\n",errno);
        return;
    }
    fprintf(stderr, "on_start done\n");
}

void on_foreground() {
    printf("on_foreground sk:%d\n",sk);

}
void on_update(void) {
    printf("on_update sk:%d\n",sk);
    struct iovec iov={0};
    iov.iov_base="hello";
    iov.iov_len=5;
    
    struct msghdr mh={0};
    mh.msg_iov=&iov;
    mh.msg_iovlen=1;
    mh.msg_name=&ia;
    mh.msg_namelen=sizeof(ia);
    
    ssize_t r=sendmsg(sk,&mh,0);

    fprintf(stderr,"sendmsg ret:%d errorno:%d\n",r, (r<0)?errno:0);

    
}
