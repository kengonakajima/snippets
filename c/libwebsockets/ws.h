#ifndef _WS_H_
#define _WS_H_

#include "inttypes.h"

class WebsocketSession
{
public:
    void *app_data_ptr;
    uint64_t id;
    static uint64_t id_gen;
    WebsocketSession() :app_data_ptr(0){
        id=id_gen++;
    }
  
};

typedef void (*WebsocketEstablishCallback)( uint64_t ws_id);
typedef void (*WebsocketReceiveCallback)( uint64_t ws_id, const char *data, uint32_t data_len );


void ws_set_establish_callback( WebsocketEstablishCallback cb );
void ws_set_receive_callback( WebsocketReceiveCallback cb );
void ws_set_app_data_ptr(uint64_t ws_id, void *ptr);
bool ws_service();
void ws_start(uint16_t port);
void ws_end();

#endif
