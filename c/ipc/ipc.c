#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include <stdio.h>

#include <time.h>
#include <errno.h>

int
main(int argc, char **argv)
{
    int i;
    key_t k;
    int ret,qid;
    struct msgbuf *msg;

    k = ftok( "/tmp/ringo",1 );
    if( k == -1 ){
        fprintf(stderr, "hogehoge\n" );
        return 1;
    }

    if( ( qid = msgget( k, IPC_CREAT | IPC_EXCL | 0666 ) ) == - 1 ){
        fprintf(stderr, "ahoahoahoa\n" );
        return 1;
    }


    msg = (struct msgbuf*)malloc(1000);
    msg->mtype = 10;        
    if( msgsnd( qid, &msg, 1000 - sizeof(long), IPC_NOWAIT)<0){
        fprintf(stderr, "mogamogamogamoahoahoahoa %s\n",
                strerror(errno));
        return 1;
    }

    memset(msg,0,1000);
    msg->mtype= 20;
    
    if( msgrcv( qid, &msg, 1000 - sizeof(long),0, IPC_NOWAIT ) <0 ){
        fprintf(stderr, "recverecte %s\n",strerror(errno));
        return 1;
    } else {
        fprintf(stderr, "received type:%d\n" ,msg->mtype);
    }
/*
    if( msgctl( qid, IPC_RMID, NULL ) < 0 ){
        fprintf(stderr, "cannot remove queue\n" );
        return 1;
    }
*/
    return 0;
}
