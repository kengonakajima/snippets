#ifndef _WS_H_
#define _WS_H_

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "cumino.h"

class Buffer
{
public:
    char *data;
    uint32_t max;
    uint32_t used;    
    Buffer(uint32_t l) {
        data=(char*)malloc(l);
        max=l;
    }
    Buffer(const char *from, uint32_t l) {
        data=(char*)malloc(l);
        max=used=l;
        memcpy(data,from,l);
    }
    void clear() {
        used=0;
    }
    void append(const char *add,uint32_t l) {
        uint32_t to_expand=0;
        if(used+l>max) to_expand=used+l-max;
        if(to_expand>0) {
            data=(char*)realloc(data,used+l);
            max=used+l;
            print("Buffer to_expand:%d",to_expand);
        }
        memcpy(data+used,add,l);
        used+=l;
    }
};
class Queue
{
public:
    Buffer **payloads;
    uint32_t capacity;
    uint32_t used;
    Queue(uint32_t capa) : capacity(capa), used(0){
        size_t sz=capa*sizeof(Buffer*);
        payloads=(Buffer**)malloc(sz);
        memset(payloads,0,sz);        
    }
    bool push(Buffer*b) {
        if(used==capacity) return false;
        payloads[used++]=b;
        return true;
    }   
};
class WebsocketSession
{
public:
    void *app_data_ptr;
    void *per_session_data;
    uint64_t id;
    static uint64_t id_gen;

    Buffer *receiving; 
    Queue *sendq;    
    WebsocketSession(void *per_session_data) :app_data_ptr(0), per_session_data(per_session_data), receiving(0) {
        id=id_gen++;
        sendq=new Queue(512);
        receiving=new Buffer(128*1024);
    }
    void receiveData(const char *in, uint32_t l, bool first, bool fin);    
};

typedef void (*WebsocketEstablishCallback)( uint64_t ws_id);
typedef void (*WebsocketCloseCallback)( uint64_t ws_id);
typedef void (*WebsocketReceiveCallback)( uint64_t ws_id, const char *data, uint32_t data_len );


void ws_set_establish_callback( WebsocketEstablishCallback cb );
void ws_set_close_callback( WebsocketCloseCallback cb );
void ws_set_receive_callback( WebsocketReceiveCallback cb );
void ws_set_app_data_ptr(uint64_t ws_id, void *ptr);

void ws_init(uint32_t max_ccu);
void ws_start(uint16_t port);
bool ws_service();
void ws_end();

#endif
