#ifndef _WS_H_
#define _WS_H_

#include "inttypes.h"

class WebsocketSession
{
public:
    void *app_data_ptr;
    void *per_session_data;
    uint64_t id;
    static uint64_t id_gen;
    WebsocketSession(void *per_session_data) :app_data_ptr(0), per_session_data(per_session_data){
        id=id_gen++;
    }
  
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
