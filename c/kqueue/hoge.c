#include <stdio.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h> // strerror
#include <errno.h> // errno
#include <unistd.h> // close
#include <fcntl.h>

int main(int argc, char **argv) {
    int svfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(22222);
    int ret=inet_pton(AF_INET,"0.0.0.0",&addr.sin_addr.s_addr);
    if(ret<0) { fprintf(stderr,"inet_pton error:%s\n",strerror(errno)); return 1; }
    ret=setsockopt(svfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    if(ret<0) { fprintf(stderr, "setsockopt error:%s\n",strerror(errno)); return 1;}
    ret=bind(svfd,(struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if(ret<0) { fprintf(stderr,"bind error:%s\n",strerror(errno)); return 1; }
    ret=listen(svfd,5);
    if(ret<0) { fprintf(stderr,"listen error:%s\n",strerror(errno)); return 1; }

    int kq=kqueue();
    
    struct kevent ev;
    EV_SET(&ev,svfd,EVFILT_READ,EV_ADD,0,0,NULL);
    ret=kevent(kq,&ev,1,NULL,0,NULL);

    struct kevent evlist[32];
    while(1) {
        int nev=kevent(kq,NULL,0,evlist,32,NULL);
        for(int i=0;i<nev;i++) {
            int fd=(int)evlist[i].ident;
            if(evlist[i].flags & EV_EOF) {
                printf("disconnect fd:%d\n",fd);
                close(fd);
            }else if(fd==svfd) {
                printf("svfdevent fd:%d\n",svfd);

                struct sockaddr_storage addr;
                socklen_t socklen = sizeof(addr);
                int connfd = accept(fd, (struct sockaddr *)&addr, &socklen);
                assert(connfd != -1);
                
                EV_SET(&ev, connfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kq, &ev, 1, NULL, 0, NULL);
                printf("Got connection!\n");
                
                int flags = fcntl(connfd, F_GETFL, 0);
                assert(flags >= 0);
                fcntl(connfd, F_SETFL, flags | O_NONBLOCK);

                // schedule to send the file when we can write (first chunk should happen immediately)
                EV_SET(&ev, connfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
                kevent(kq, &ev, 1, NULL, 0, NULL);

            } else if(evlist[i].filter == EVFILT_READ) {
                char buf[10]={};
                size_t br = recv(fd,buf,sizeof(buf)-1,0);
                printf("data got len:%zu %s\n",br,buf);
                
            } else if(evlist[i].filter == EVFILT_WRITE) {
                char buf[4] = { 'h', 'o', 'g', 'e' };
                int wr = send(fd,buf,4,0);
                if(wr<=0) {
                    printf("wr<0 for fd:%d\n",fd);
                    if(errno==EAGAIN) {
                        EV_SET(&ev,fd,EVFILT_WRITE,EV_ADD | EV_ONESHOT,0,0,NULL);
                        kevent(kq,&ev,1,NULL,0,NULL);
                    } else {
                        printf("closed?");
                        close(fd);
                    }
                } else {
                    printf("wr:%d\n",wr);
                }
            }
        }
    }
}
