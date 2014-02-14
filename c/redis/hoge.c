#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <assert.h>

#include "hiredis.h"
#include "async.h"
#include "adapters/libev.h"

void getCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = r;
    if (reply == NULL) return;
    printf("argv[%s]: %s\n", (char*)privdata, reply->str);

    /* Disconnect after receiving the reply to GET */
    //    redisAsyncDisconnect(c);
}

void connectCallback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Connected...\n");
}

void disconnectCallback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
}

int main (int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);

    redisContext *rc;
    struct timeval timeout = { 1,500000 };    
    rc = redisConnectWithTimeout( "localhost", 6379, timeout );
    assert(rc && rc->err==0);

    int n = 1000000;
    for(int i=0;i<n;i++) {
        char hoge[100];
        snprintf(hoge,sizeof(hoge), "ZADD ztest %d hogehoge%d", 10+i,i );
        redisReply *reply = (redisReply*) redisCommand( rc, hoge );
        //        fprintf( stderr, "RRR: %d '%s' int:%lld", reply->type, reply->str, reply->integer );
        if(i%100==0){
            fprintf(stderr,".");
        }
    }
    
    
#if 0    
    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
    if (c->err) {
        /* Let *c leak for now... */
        printf("Error: %s\n", c->errstr);
        return 1;
    }

    redisLibevAttach(EV_DEFAULT_ c);
    redisAsyncSetConnectCallback(c,connectCallback);
    redisAsyncSetDisconnectCallback(c,disconnectCallback);
    for(int i=0;i<1000;i++) {
        //        redisAsyncCommand(c, NULL, NULL, "SET key %b", argv[argc-1], strlen(argv[argc-1]));
        char hoge[100];
        snprintf(hoge,sizeof(hoge), "pooooooo%d", i );
        redisAsyncCommand(c, NULL, NULL, "SET key %s", hoge );
        redisAsyncCommand(c, getCallback, (char*)"end-1", "GET key");
    }
    printf( "loopend" );
    ev_loop(EV_DEFAULT_ 0);
#endif    
    return 0;
}
