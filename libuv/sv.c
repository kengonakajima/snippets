#include "uv.h"
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

typedef struct {

    char buf[1024];
    size_t len;

    void *data;
    
} htparser_t;

void dump(char*p,size_t n){
    int i;    
    for(i=0;i<n;i++){
        fprintf(stderr, "%d ", p[i]);
    }    
}
void htparser_init( htparser_t *htp )
{
    memset(htp->buf, 0,sizeof(htp->buf));
    htp->len=0;
}



#define RESPONSE \
  "HTTP/1.1 200 OK\r\n" \
  "Content-Type: text/plain\r\n" \
  "Content-Length: 12\r\n" \
  "\r\n" \
  "hello world\n"


uv_buf_t refbuf;

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
    char recvbuf[1024];
    size_t recvlen;
} write_req_t;

typedef struct {
    uv_tcp_t handle; // uv_handle_t にしてしまうと、構造体の後ろが壊される。。無理矢理オブジェクト指向してるため。
    htparser_t htparser;
    uv_write_t write_req;
} htcli_t;


uv_loop_t* loop;
uv_tcp_t tcpEchoServer;
uv_handle_t* echoServer;
uv_tcp_t tcpHTTPServer;
uv_handle_t* httpServer;

void http_on_close(uv_handle_t* peer) {
    free(peer);
}

void http_after_write( uv_write_t *req, int status ) {
  uv_close((uv_handle_t*)req->handle, http_on_close);
}

void htparser_add_data( htparser_t *htp, char *buf, ssize_t nr ) {
    //    fprintf(stderr, "htp: %p nr:%d %s\n", htp, (int)nr, buf );
    if( htp->len + nr < sizeof(htp->buf)-1 ){
        memcpy( htp->buf + htp->len, buf, nr );
        htp->len += nr;
        htp->buf[ htp->len ] = 0;
        //        fprintf(stderr, "accum:%s %d\n", htp->buf, (int)htp->len);
#define HEADSTR1 "GET / HTTP/1.1\r\n"
#define HEADSTR0 "GET / HTTP/1.0\r\n"                                
        if( strncmp( htp->buf, HEADSTR0, strlen(HEADSTR0))==0 ||
            strncmp( htp->buf, HEADSTR1, strlen(HEADSTR1))==0 ){
            htcli_t* htcl = htp->data;
            uv_write(&htcl->write_req, (uv_stream_t*)&htcl->handle, &refbuf, 1, http_after_write);
        } 
    }
}

void echo_on_server_close(uv_handle_t* handle) {
    assert(handle == echoServer);
}

void echo_on_close(uv_handle_t* peer) {
    free(peer);
}

void echo_after_shutdown(uv_shutdown_t* req, int status) {
    uv_close((uv_handle_t*)req->handle, echo_on_close);
    free(req);
}
void http_after_shutdown(uv_shutdown_t* req, int status) {
    uv_close((uv_handle_t*)req->handle, http_on_close);
    free(req);
}

void echo_after_write(uv_write_t* req, int status) {
  write_req_t* wr;

  if (status) {
    uv_err_t err = uv_last_error(loop);
    fprintf(stderr, "uv_write error: %s\n", uv_strerror(err));
    assert(0);
  }

  wr = (write_req_t*) req;

  /* Free the read/write buffer and the request */
  free(wr->buf.base);
  free(wr);
}


void echo_after_read(uv_stream_t* handle, ssize_t nread, uv_buf_t buf) {
    int i;
    write_req_t *wr;
    uv_shutdown_t* req;
    if (nread < 0) {
        /* Error or EOF */
        assert (uv_last_error(loop).code == UV_EOF);
        if (buf.base) {
            free(buf.base);
        }
        req = (uv_shutdown_t*) malloc(sizeof *req);
        uv_shutdown(req, handle, echo_after_shutdown);

        return;
    }

    if (nread == 0) {
        /* Everything OK, but nothing read. */
        free(buf.base);
        return;
    }
    /* close command */
    for (i = 0; i < nread; i++) {
        if (buf.base[i] == 'Q') {
            if (i + 1 < nread && buf.base[i + 1] == 'S') {
                free(buf.base);
                uv_close((uv_handle_t*)handle, echo_on_close);
                return;
            } else {
                uv_close(echoServer, echo_on_server_close);
            }
        }
    }

    wr = (write_req_t*) malloc(sizeof *wr);
    wr->buf = uv_buf_init(buf.base, nread);
    wr->recvlen=0;
    if (uv_write(&wr->req, handle, &wr->buf, 1, echo_after_write)) {
        assert(!"uv_write failed");
    }
}
void http_after_read(uv_stream_t*handle, ssize_t nread, uv_buf_t buf) {
    uv_shutdown_t* req;
    size_t parsed;
    htcli_t *cli = handle->data;
    
    if(nread<0){
        assert( uv_last_error(loop).code == UV_EOF);
        if(buf.base){ free(buf.base); }
        req = (uv_shutdown_t*) malloc( sizeof( *req ) );
        uv_shutdown(req,handle, http_after_shutdown);
        return;
    }
    if( nread==0){
        free(buf.base);
        return;
    }
    /* data is coming */
    htparser_add_data( &cli->htparser, buf.base, nread );
}



uv_buf_t echo_alloc(uv_handle_t* handle, size_t suggested_size) {
    //    fprintf(stderr, "suggested_size: %d\n", (int)suggested_size );
    return uv_buf_init(malloc(suggested_size), suggested_size);
}

uv_buf_t http_alloc(uv_handle_t* handle, size_t suggested_size) {
    return uv_buf_init(malloc(suggested_size), suggested_size);
}

void echo_on_connection(uv_stream_t* server, int status) {
    uv_stream_t* stream;
    int r;
    fprintf(stderr, "echo ");

    if (status != 0) {
        fprintf(stderr, "Connect error %d\n",uv_last_error(loop).code);
    }
    assert(status == 0);

    stream = malloc(sizeof(uv_tcp_t));
    assert(stream != NULL);
    r = uv_tcp_init(loop, (uv_tcp_t*)stream);
    assert(r == 0);

    /* associate server with stream */
    stream->data = server;

    r = uv_accept(server, stream);
    assert(r == 0);


    r = uv_read_start(stream, echo_alloc, echo_after_read);
    assert(r == 0);
}

void http_on_connection(uv_stream_t* server, int status) {
    int r;
    assert(status==0);

    htcli_t *htcl;

    htcl = malloc( sizeof( htcli_t) );
    r = uv_tcp_init( loop, (uv_tcp_t*) &htcl->handle );
    assert(r==0);
    r = uv_accept(server, (uv_stream_t*) &htcl->handle );
    assert(r==0);
    htcl->handle.data = htcl;
    htcl->htparser.data = htcl;
    htparser_init( &htcl->htparser );
    r = uv_read_start( (uv_stream_t*) &htcl->handle, http_alloc, http_after_read );
    assert(r==0);
}
    

void startEchoServer(uv_loop_t*loop)
{
    struct sockaddr_in addr = uv_ip4_addr("0.0.0.0", 22222);
    int r;

    echoServer = (uv_handle_t*)&tcpEchoServer;

    r = uv_tcp_init(loop, &tcpEchoServer);
    if (r) {
        fprintf(stderr, "Socket creation error\n");
        return;
    }

    r = uv_tcp_bind(&tcpEchoServer, addr);
    if (r) {
        fprintf(stderr, "Bind error\n");
        return;
    }

    r = uv_listen((uv_stream_t*)&tcpEchoServer, SOMAXCONN, echo_on_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(uv_last_error(loop)));
        return;
    }
    
}
void startHTTPServer(uv_loop_t*loop)
{
    struct sockaddr_in addr = uv_ip4_addr("0.0.0.0", 22280 );
    int r;
    httpServer = (uv_handle_t*)&tcpHTTPServer;
    r = uv_tcp_init( loop, &tcpHTTPServer );
    if(r){
        fprintf(stderr, "sk cr er");
        return;
    }
    r = uv_tcp_bind(&tcpHTTPServer, addr);
    if(r){
        fprintf(stderr,"bind error http");
        return;
    }
    r = uv_listen((uv_stream_t*)&tcpHTTPServer, SOMAXCONN, http_on_connection );
    if(r){
        fprintf(stderr,"listen error http");
        return;
    }
}

int main(int argc, char **argv )
{
    loop = uv_default_loop();

    refbuf.base = RESPONSE;
    refbuf.len = sizeof(RESPONSE);
    
    startEchoServer(loop);
    startHTTPServer(loop);

    uv_run(loop);
        
    return 0;

}

