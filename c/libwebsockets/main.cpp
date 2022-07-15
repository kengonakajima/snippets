#include "ws.h"
#include "cumino.h"

class Client
{
public:
    uint64_t ws_id;
    Client(uint64_t ws_id) :ws_id(ws_id) {
        
    }
    ~Client() {
        
    }
};

void on_establish(uint64_t ws_id) {
    Client *cl = new Client(ws_id);
    ws_set_app_data_ptr(ws_id,cl);
    print("on_establish. %llx",ws_id);
}
void on_close(uint64_t ws_id) {
    print("on_close. %llx",ws_id);    
}
void on_receive(uint64_t ws_id, const char *data, uint32_t data_len ) {
    print("on_receive len:%u",data_len);
}
int main(int argc, char **argv) {
    ws_init(100);
    ws_set_establish_callback(on_establish);
    ws_set_close_callback(on_close);    
    ws_set_receive_callback(on_receive);
    ws_start(18001);
    while(1) {
        bool keep=ws_service();
        if(!keep)break;
    }
    ws_end();
    return 0;
}
