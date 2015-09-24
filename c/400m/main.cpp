#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "cumino.h"

#define PATH "/tmp/hoge"
#define UNITSIZE (12288)  // to be more real
#define UNITNUM (16384)
#define TOTALSIZE (UNIT*UNITNUM)

void createBigFile() {
    FILE *fp = fopen( PATH, "wb" );
    int ret;
    ret = fseek( fp, UNITSIZE * (UNITNUM-1), SEEK_SET );
    assert(ret==0);
    char buf[UNITSIZE];
    memset( buf, 0xfd, sizeof(buf) );

    ret = fwrite( buf, 1, UNITSIZE, fp );
    assert( ret == UNITSIZE );
    fclose(fp);
}

int main() {

    double st = now();
    fprintf(stderr, "create..\n");
    createBigFile();
    double et = now();
    fprintf(stderr, "done %f\n", et-st );


    int ret;
    int n = 10000;
    char buf[UNITSIZE];
    memset( buf, 0xff, UNITSIZE );

    double fp_write_time, fd_write_time;
    double fp_close_time, fd_close_time;    
    
    // Measure fp
    FILE *fp = fopen( PATH, "wb" );
    assert(fp);
    fprintf(stderr, "fp-write..\n");    
    st = now();
    for(int i=0;i<n;i++) {
        size_t ofs = irange(0,UNITNUM) * UNITSIZE;
        ret = fseek( fp, ofs, SEEK_SET );
        assert(ret==0);
        ret = fwrite( buf, 1, UNITSIZE, fp );
        assertmsg(ret==UNITSIZE, "fwrite ret:%d %s",ret, strerror(errno));
    }
    et = now();
    fp_write_time = et-st;
    fprintf(stderr, "done. %f\n", et-st );
    fprintf(stderr, "close..\n" );
    st = now();
    fclose(fp);
    et = now();
    fp_close_time = et-st;
    fprintf(stderr, "done. %f\n", et-st);

    // Measure fd
    int fd = open( PATH, O_RDWR );
    assert(fd>0);
    fprintf(stderr, "fd-write..\n");
    st = now();
    for(int i=0;i<n;i++) {
        size_t ofs = irange(0,UNITNUM) *UNITSIZE;
        ret = lseek( fd, ofs, SEEK_SET );
        assert(ret==ofs);
        ret = write( fd, buf, UNITSIZE );
        assert(ret==UNITSIZE);
    }
    et = now();
    fd_write_time = et-st;
    fprintf(stderr, "done. %f\n", et-st);
    fprintf(stderr, "fd-close..\n" );
    st = now();
    close(fd);
    et = now();
    fd_close_time = et-st;
    fprintf(stderr, "done. %f\n", et-st);

    fprintf(stderr, "FP: write:%f close:%f FD: write:%f close:%f Ratio: write:%f close:%f",
            fp_write_time, fp_close_time, fd_write_time, fd_close_time,
            fp_write_time / fd_write_time,
            fp_close_time / fd_close_time
            );
    return 0;
}
