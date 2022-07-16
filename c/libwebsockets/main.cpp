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

uint64_t g_last_ws_id;
void on_establish(uint64_t ws_id) {
    //    Client *cl = new Client(ws_id);
    //    ws_set_app_data_ptr(ws_id,cl);
    print("on_establish. %llx",ws_id);
    g_last_ws_id=ws_id;
}
void on_close(uint64_t ws_id) {
    print("on_close. %llx",ws_id);    
}
void on_receive(uint64_t ws_id, const char *data, uint32_t data_len ) {
    //    bool ret=ws_send(ws_id,data,data_len);
    print("on_receive len:%u",data_len);
    ws_send(ws_id,data,data_len);
}
int main(int argc, char **argv) {
    ws_init(100);
    ws_set_establish_callback(on_establish);
    ws_set_close_callback(on_close);    
    ws_set_receive_callback(on_receive);
    ws_start(18001);
    while(1) {
        static uint64_t cnt=0;
        cnt++;
        if(cnt%10000==0) {
            print("loop %f g_last_ws_id:%llx",now(),g_last_ws_id);
            ws_send(g_last_ws_id,"hello",5);
#if 0            
            const int N=3000000;
            char msg[N];
            for(int i=0;i<N;i++)msg[i]='a';
            msg[N-1]=0;
            msg[N-2]='b';            
            ws_send(g_last_ws_id,msg,N,true);
#endif            
        }
        
        bool keep=ws_service();
        if(!keep)break;
    }
    ws_end();
    return 0;
}
