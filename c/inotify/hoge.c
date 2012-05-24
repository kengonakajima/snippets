#include <sys/inotify.h>
#include <stdio.h>

int main() {
    int fd;
    fd = inotify_init();
    if(fd<0){
        fprintf(stderr,"err\n");
        return 1;
    }

    int wd;
    wd = inotify_add_watch( fd, "/tmp", IN_CREATE|IN_MODIFY|IN_DELETE );
    if(wd<0){
        fprintf(stderr,"err2\n");
        return 1;
    }

    while(1){
        int r;
        char buf[4096];
        fprintf(stderr, "read..");
        r = read( fd, buf, sizeof(buf) );
        if(r>0){
            struct inotify_event *ptr = (struct inotify_event*)buf;
            fprintf(stderr, "%s\n",ptr->name );
        }                  
    }
        
    int r;
    r=inotify_rm_watch(fd,wd);
    if(r<0){
        fprintf(stderr,"err3\n");
        return 1;        
    }

    return 0;
}
