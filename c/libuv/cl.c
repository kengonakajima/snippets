// libuv 1.8.0 example

#include <uv.h>
#include <stdio.h>
#include <stdlib.h>


void alloc_buffer( uv_handle_t *handle, size_t suggested_size, uv_buf_t *outbuf ) {
    *outbuf = uv_buf_init( (char*) malloc(suggested_size), suggested_size );
}

void on_close(uv_handle_t* handle) {
  printf("closed.");
}
void on_write(uv_write_t* req, int status) {
    if (status) {
        fprintf(stderr, "uv_write error: %s\n", uv_strerror(status));
		return;
    }
    printf("wrote.\n");
	//uv_close((uv_handle_t*)req->handle, on_close);
    free(req->data);
    free(req);
}

void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t *buf) {
	if(nread >= 0) {
		//printf("read: %s\n", tcp->data);
		printf("read len:%zu\n", nread );
        for(int i=0;i<nread;i++) {
            printf("%02x ", buf->base[i] );
        }
        uv_write_t *wreq = (uv_write_t*)malloc(sizeof(uv_write_t));
        uv_buf_t wb;
        wb.base = (char*)malloc(nread);
        wb.len = nread;
        memcpy( wb.base, buf->base, nread );
        wb.base[0] = 0x40 + (rand()%20);        
        uv_write( wreq, tcp, &wb, 1, on_write );
	} else {
		//we got an EOF
        uv_close((uv_handle_t*)tcp, on_close);
	}

	//cargo-culted
	free(buf->base);
}


void on_connect(uv_connect_t* connection, int status) {
	printf("connected.\n");

	uv_stream_t* stream = connection->handle;

	uv_buf_t buffer[] = {
		{.base = "hello", .len = 5},
		{.base = "world", .len = 5}
	};

	uv_write_t *request = (uv_write_t*)malloc(sizeof(uv_write_t));

	uv_write( request, stream, buffer, 2, on_write);
	uv_read_start( stream, alloc_buffer, on_read);
}



int main() {
    uv_loop_t *loop = uv_default_loop();

    uv_tcp_t *client = (uv_tcp_t*)malloc( sizeof(uv_tcp_t) );
    uv_tcp_init(loop, client);

    uv_connect_t  *connect = (uv_connect_t*)malloc( sizeof(uv_connect_t));

    struct sockaddr_in addr;
    uv_ip4_addr("127.0.0.1", 22222, &addr);

    int r;
    r = uv_tcp_connect( connect, client, (const struct sockaddr*)&addr, on_connect );
    if(r!=0) {
        fprintf(stderr, "error: uv_tcp_connect: ret:%d",r);
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}

