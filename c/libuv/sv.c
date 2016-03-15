// libuv 1.8.0 example

#include <uv.h>
#include <stdio.h>
#include <stdlib.h>

void echo_write(uv_write_t *req, int status) {
  if (status == -1) {
    fprintf(stderr, "Write error!\n");
  }
  char *base = (char*) req->data;
  free(base);
  free(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
  if (nread == -1) {
    fprintf(stderr, "Read error!\n");
    uv_close((uv_handle_t*)client, NULL);
    return;
  }

  uv_buf_t wbuf;
  wbuf.base = buf->base;
  wbuf.len = nread;
  
  uv_write_t *write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
  write_req->data = (void*)wbuf.base;

  uv_write(write_req, client, &wbuf, 1, echo_write);
}



void alloc_buffer( uv_handle_t *handle, size_t suggested_size, uv_buf_t *outbuf ) {
    *outbuf = uv_buf_init( (char*) malloc(suggested_size), suggested_size );
}


void on_new_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init( uv_default_loop(), client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);
#if 1 // write before any read happens
        uv_buf_t wbuf;
        wbuf.base = malloc(10);
        memcpy( wbuf.base, "hogehoge\r\n", 10 );
        wbuf.len = 10;
        uv_write_t *write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
        write_req->data = (void*)wbuf.base;
        uv_write(write_req, (uv_stream_t*)client, &wbuf, 1, echo_write );
#endif        
    } else {
        uv_close((uv_handle_t*) client, NULL);
    }
}


int main() {
    uv_loop_t *loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", 22222, &addr);

    uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*) &server, 10, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}
