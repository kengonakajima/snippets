#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include <sys/poll.h>

//#include <netdb.h>

int main(int argc, char **argv) {
    int svfd = socket(AF_INET,SOCK_STREAM,0);
    int port = 22222;
    struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = INADDR_ANY;

    int on=1;
    int rc = setsockopt(svfd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
    if(rc!=0)return 1;
    
	int ret = bind(svfd,(struct sockaddr *) &sa,sizeof(sa));
	if( ret < 0){
		printf("bind error.errno%d\n",errno);
        return 1;
	}

	ret = listen(svfd,5);
	if(ret < 0){
		fprintf(stderr,"listen error: %d\n",errno);
        return 1;
	}

    int loopcnt=0;
    struct pollfd pfds[100];
    pfds[0].fd=svfd;
    pfds[0].events = POLLIN | POLLOUT | POLLERR;
    pfds[0].revents = 0;
    int pfn=1;
    
    while(1) {        
        usleep(1000*50);
        
        loopcnt++;
        int pr = poll(pfds,pfn,0);
        if(loopcnt%1000==0) fprintf(stderr,"[%d] pr:%d fd:%d pfn:%d\n",loopcnt,pr, pfds[0].fd,pfn);
        if(pr==0)continue;
        fprintf(stderr,".");
        for(int i=0;i<pfn;i++) {
            struct pollfd *p=&pfds[i];
            if(p->revents==0)continue;
            if(p->fd<=0)continue;
            fprintf(stderr,"Ev:%d %d %d %d\n",p->revents, POLLIN,POLLOUT,POLLERR);
            if(p->revents & POLLOUT) {
                fprintf(stderr,"out:%d\n",i);
            }
            if(p->revents & POLLERR) {
                fprintf(stderr,"err:%d\n",i);
            }
            if(p->revents & POLLHUP) {
                fprintf(stderr,"hup:%d\n",i);
                close(p->fd);
                p->fd=-1;
            }
            if(p->revents & POLLIN) {
                if(p->fd==svfd) {
                    fprintf(stderr,"sv in %d\n",i);
                    struct sockaddr_in peersa;
                    socklen_t peersalen=sizeof(peersa);
                    int nfd=accept(svfd, (struct sockaddr *)&peersa , &peersalen);
                    if(nfd>0) {
                        fprintf(stderr,"accept ok svfd:%d nfd:%d pfn:%d\n",svfd,nfd,pfn);
                        pfds[pfn].fd=nfd;
                        pfds[pfn].events = POLLIN | POLLOUT | POLLERR;
                        pfds[pfn].revents = 0;
                        pfn++;
                    } else {
                        fprintf(stderr,"accept error %d\n",errno);
                    }
                } else {
                    char buf[100];
                    ssize_t rr=recv(p->fd,(void*)buf,sizeof(buf),0);
                    fprintf(stderr,"recv fd=%d rr:%ld\n",p->fd,rr);
                    if(rr>0) {
                        rr=send(p->fd,buf,rr,0);
                        fprintf(stderr,"send fd=%d rr:%ld\n",p->fd,rr);
                    }
                }
            }
        }

    }


}
