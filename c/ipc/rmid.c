#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <stdio.h>

#include <time.h>

int
main(int argc, char **argv)
{
    key_t k;
    int ret,qid;


    k = ftok( "/tmp/ringo",1 );
    if( k == -1 ){
        fprintf(stderr, "hogehoge\n" );
        return 1;
    }

    if( ( qid = msgget( k,0 )) == - 1 ){
        fprintf(stderr, "ahoahoahoa\n" );
        return 1;
    }	
    if( msgctl( qid, IPC_RMID, NULL ) < 0 ){
        fprintf(stderr, "cannot remove queue\n" );
        return 1;
    }
    return 0;
}

