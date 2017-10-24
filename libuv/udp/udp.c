#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

uv_loop_t *loop;
uv_udp_t send_socket;
uv_udp_t recv_socket;

int port =22222;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *outbuf ) {
    *outbuf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void on_read(uv_udp_t *req, long nread, const uv_buf_t *buf, 
             const struct sockaddr *addr, unsigned flags) {
    if (nread == -1) {
        fprintf(stderr, "Read error!\n");
        uv_close((uv_handle_t*) req, NULL);
        free(buf->base);
        return;
    }

    char sender[17] = { 0 };
    uv_ip4_name((struct sockaddr_in*) addr, sender, 16);
    fprintf(stderr, "Recv from %s\n", sender);

    int i;
    for(i=0;i<nread;i++) {
        fprintf(stderr,"read bytes:%d %d\n",i,buf->base[i]);
    }
    free(buf->base);
    uv_udp_recv_stop(req);
}

void on_send(uv_udp_send_t *req, int status) {
    if (status == -1) {
        fprintf(stderr, "Send error!\n");
        return;
    } else {
        fprintf(stderr, "send ok\n");
    }
}

int main() {
    loop = uv_default_loop();

    uv_udp_init(loop, &recv_socket);
    int ret;
    struct sockaddr_in recv_addr;
    ret = uv_ip4_addr("0.0.0.0",port,&recv_addr);
  
    uv_udp_bind(&recv_socket, (struct sockaddr*)&recv_addr, 0);
    uv_udp_recv_start(&recv_socket, alloc_buffer, on_read);

    uv_udp_init(loop, &send_socket);
    struct sockaddr_in sender_addr;
    uv_ip4_addr("0.0.0.0", 0,&sender_addr);
    uv_udp_bind(&send_socket, (struct sockaddr*)&sender_addr,0);
    uv_udp_set_broadcast(&send_socket, 1);

    uv_udp_send_t send_req;
    uv_buf_t msg;
    alloc_buffer((uv_handle_t*)&send_req,32,&msg);
    int i;
    for(i=0;i<32;i++) msg.base[i]=i;

    struct sockaddr_in dest_addr;
    uv_ip4_addr("255.255.255.255", port, &dest_addr );
    fprintf(stderr, "sending\n");
    uv_udp_send(&send_req, &send_socket, &msg, 1, (struct sockaddr*)&dest_addr, on_send);

    return uv_run(loop, UV_RUN_DEFAULT);
}
