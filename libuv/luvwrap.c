/*
  luv : libuv lua binding on luajit FFI
 */
#include "uv.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

#define DEBUG 0
#if DEBUG
#define LOG(x) (x)
#else
#define LOG(x)
#endif

void *luv_default_loop() { return uv_default_loop(); }
void luv_run(void*loop) { uv_run(loop); }



typedef struct {
    int id;
    void *stream;
    //    void (*afterreadcb)(void*h,int id,int nread,void *buf_base, int buf_len); // notused for performance
    //    void (*afterwritecb)(void *h, int status); // notused for performance
    void (*aftershutdowncb)(void *sr, int status);
    uv_shutdown_t *shutdownreq; // libuv spec: you can't use any types other than uv_shutdown_t
    void (*afterclosecb)(void *h);
    void (*afterconnectcb)(void *h, int id, int status );
    uv_connect_t *connectreq;
} cbhist_t;
#define CBN 1024
cbhist_t cbs[CBN];  
cbhist_t* cbhist_find_by_handle( uv_handle_t *h){
    int i;
    for(i=0;i<CBN;i++){ // TODO: for c10k, don't scan array! 
        if( cbs[i].id != 0 && cbs[i].stream == h ) {
            return & cbs[i];
        }
    }
    return NULL;
}
cbhist_t *cbhist_find_by_shutdownreq( uv_shutdown_t *s ) {
    int i;
    for(i=0;i<CBN;i++){ // TODO: for c10k, don't scan array! 
        if( cbs[i].id != 0 && cbs[i].shutdownreq == s ) {
            return & cbs[i];
        }
    }
    return NULL;    
}
cbhist_t *cbhist_find_by_connectreq( uv_connect_t *c ) {
    int i;
    for(i=0;i<CBN;i++){ // TODO: for c10k, don't scan array! 
        if( cbs[i].id != 0 && cbs[i].connectreq == c ) {
            return & cbs[i];
        }
    }
    return NULL;        
}

int maxInd=-1;
cbhist_t *cbhist_alloc() {
    int i;
    static int gid=1;    
    for(i=0;i<CBN;i++){ // TODO: for c10k, don't scan array! 
        if( cbs[i].id == 0){
            cbs[i].id = gid++;
            LOG(fprintf(stderr, "cbhist_alloc: allocated. id:%d ptr:%p\n", cbs[i].id, & cbs[i] ));
            if(i>maxInd){
                maxInd=i;
                fprintf(stderr,"max:%d\n",maxInd);
            }
            return & cbs[i];
        }
    }
    assert(!"no cb hist space");
    return NULL;
}

void cbhist_free( cbhist_t *h ) {
    int i;
    for(i=0;i<CBN;i++){
        if(cbs[i].id == h->id ){
            cbs[i].id=0;
            return;
        }
    }
    assert(!"not found");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


void* luv_tcp_init( void*loop ) {
    uv_tcp_t *tcp = malloc( sizeof *tcp); // TODO:gc
    LOG(fprintf(stderr, "luv_tcp_init: tcp:%p\n",tcp));
    int r = uv_tcp_init(loop, tcp);
    assert(r==0);
    cbhist_t *cbh = cbhist_find_by_handle((uv_handle_t*)tcp);
    assert(!cbh);
    cbh = cbhist_alloc();
    cbh->stream = tcp;

    return tcp;
}
int luv_tcp_id( void *tcp ) {
    cbhist_t *cbh = cbhist_find_by_handle((uv_handle_t*)tcp);
    if(cbh){
        return cbh->id;
    } else {
        return -1;
    }
}

int luv_tcp_bind( void*svh, char *addrb, int port ) {
    LOG(fprintf(stderr,"luv_tcp_bind: called. svh:%p addrb:%s port:%d\n",svh,addrb,port));
    struct sockaddr_in addrin = uv_ip4_addr( addrb, port );
    int r = uv_tcp_bind( svh, addrin );
    return r;
}

int luv_tcp_listen( void *tcp, int maxcon, void *cb ) {
    LOG(fprintf(stderr,"luv_tcp_listen: called. tcp:%p maxcon:%d cb:%p\n", tcp,maxcon,cb));
    int r = uv_tcp_listen( tcp, maxcon, cb );
    return r;
}


uv_buf_t alloccb_wrap(uv_handle_t* handle, size_t suggested_size) {
    cbhist_t *cbh = cbhist_find_by_handle(handle);
    assert(cbh);
    char *base = malloc( suggested_size ); // freed in afterreadcb_wrap   
    LOG(fprintf(stderr, "alloccb_wrap: found cbhist:%p new buf:%p\n",cbh,base ));
    return uv_buf_init(base, suggested_size); 
}
void (*_luv_after_read_callback_wrapper)(void*handle, int id,int nread, char *bufbase, int buflen ) ;
void luv_set_after_read_callback_wrapper( void (*f)(void*handle,int,int,char*,int) ) {
    _luv_after_read_callback_wrapper = f;
}

void afterreadcb_wrap(uv_stream_t* handle, ssize_t nread, uv_buf_t buf) {
    cbhist_t *cbh = cbhist_find_by_handle((uv_handle_t*)handle);
    assert(cbh);
    LOG(fprintf(stderr, "afterreadcb_wrap: found:%p nread:%d buf.base:'%s', buf.len:%d cb:%p\n",cbh, (int)nread, buf.base, (int)buf.len, cbh->afterreadcb ));
    assert(_luv_after_read_callback_wrapper);
    _luv_after_read_callback_wrapper(handle,cbh->id,nread,buf.base,buf.len);
    free( buf.base );
}

int luv_read_start( void *stream ) {
    cbhist_t *cbh = cbhist_find_by_handle(stream);
    if(!cbh){
        cbh = cbhist_alloc();
    }
    assert(cbh);
    
    int r = uv_read_start( stream, alloccb_wrap, afterreadcb_wrap );
    cbh->stream = stream;

    LOG(fprintf(stderr, "luv_read_start: uv_read_start ret:%d hist:%p\n", r, cbh ));
    return r;
}
int luv_accept( void *server, void *stream ) {
    int r = uv_accept( server, stream );
    return r;
}

typedef struct {
    uv_write_t req;
    uv_handle_t *h;
    uv_buf_t buf;
} writereq_t;

void (*_luv_after_write_callback_wrapper)(void*handle,int status);

void afterwritecb_wrap( uv_write_t *wr, int status ) {
    writereq_t *wreq = (writereq_t*)wr; // tricky.
    free( wreq->buf.base );
    free( wreq );
 
    cbhist_t *cbh = cbhist_find_by_handle(wreq->h);
    assert(cbh);
    LOG(fprintf(stderr, "afterwritecb_wrap found:%p fn:%p\n",cbh, cbh->afterwritecb ));
    assert(_luv_after_write_callback_wrapper);
    _luv_after_write_callback_wrapper( wreq->h, status );
}

void luv_set_after_write_callback_wrapper( void (*f)(void*handl, int status) ) {
    _luv_after_write_callback_wrapper = f;
}
int luv_write(uv_stream_t* handle, void *tosend, int buflen ) {
    cbhist_t *cbh = cbhist_find_by_handle((uv_handle_t*)handle);
    assert(cbh);
    LOG(fprintf(stderr, "luv_write-c: found:%p buflen:%d\n",cbh, buflen ));
    writereq_t *wreq = malloc(sizeof*wreq); // freed in afterwritecb_wrap
    LOG(fprintf(stderr, "luv_write-c: wreq buf:%p\n", wreq ));
    wreq->h = (uv_handle_t*)handle;
    char *base = malloc(buflen); // freed in afterwritecb_wrap
    LOG(fprintf(stderr, "luv_write-c: base:%p buflen:%d\n", base, buflen ));
    memcpy( base, tosend, buflen );
    wreq->buf = uv_buf_init( base, buflen );
    
    return uv_write( &wreq->req, handle, & wreq->buf, 1, afterwritecb_wrap );
}

void aftershutdowncb_wrap( uv_shutdown_t *sr, int status ) {
    cbhist_t *cbh = cbhist_find_by_shutdownreq(sr);
    assert(cbh);
    LOG(fprintf(stderr, "aftershutdowncb_wrap: found:%p\n",cbh));        
    if(cbh->aftershutdowncb) cbh->aftershutdowncb( cbh->stream, status );
    free(sr);
}

int luv_shutdown( uv_stream_t *stream, void (*cb)(void*,int) ) {
    cbhist_t *cbh = cbhist_find_by_handle((uv_handle_t*)stream);
    assert(cbh);
    uv_shutdown_t *sr = malloc( sizeof *sr );// freed in aftershutdowncb_wrap
    cbh->shutdownreq = sr;
    cbh->aftershutdowncb = cb;
    return uv_shutdown(sr,stream,aftershutdowncb_wrap);
}

void afterclosecb_wrap( uv_handle_t *handle ) {
    cbhist_t *cbh = cbhist_find_by_handle(handle);
    assert(cbh);
    LOG(fprintf(stderr, "afterclosecb_wrap: found:%p cb:%p\n",cbh, cbh->afterclosecb));
    if(cbh->afterclosecb) cbh->afterclosecb(handle);
    cbhist_free(cbh);
}

void luv_close( uv_handle_t *handle, void (*cb)(void*h) ) {
    cbhist_t *cbh = cbhist_find_by_handle(handle);
    assert(cbh);
    cbh->afterclosecb = cb;
    return uv_close( handle, afterclosecb_wrap );
}

void afterconnectcb_wrap( uv_connect_t *cr, int status ) {
    cbhist_t *cbh = cbhist_find_by_connectreq(cr);
    assert(cbh);
    LOG(fprintf(stderr, "calling afterconnectcb: id:%d, status:%d\n", cbh->id, status ));
    if(cbh->afterconnectcb) cbh->afterconnectcb( cbh->stream, cbh->id, status );
    free(cr);
}

int luv_tcp_connect( uv_tcp_t *tcp, char *svaddr, int svport, void (*cb)(void*,int,int) ) {
    cbhist_t *cbh = cbhist_find_by_handle((uv_handle_t*)tcp);
    if(!cbh) cbh = cbhist_alloc();
    assert(cbh);

    cbh->stream = tcp;
    cbh->afterconnectcb = cb;
    uv_connect_t *cr = malloc(sizeof *cr); // freed in afterconnectcb_wrap
    LOG(fprintf(stderr, "luv_tcp_connect: cr:%p\n",cr));
    cbh->connectreq = cr;
    struct sockaddr_in svaddrin = uv_ip4_addr( svaddr, svport );
    int r = uv_tcp_connect(cr,tcp,svaddrin,afterconnectcb_wrap );
    return r;
}

uv_timer_t * luv_timer_init(uv_loop_t*loop) {
    uv_timer_t *t = malloc( sizeof *t); // TODO:gc
    LOG(fprintf(stderr, "luv_timer_init: t:%p\n",t));
    int r = uv_timer_init( loop, t );
    if(r)return NULL; else return t;
}

int luv_timer_start( uv_timer_t *t, void (*cb)( uv_timer_t *,int), double timeout, double repeat) {
    int r;
    r = uv_timer_start( t, cb, timeout, repeat );
    return r;
}
int luv_timer_stop( uv_timer_t *t){
    return uv_timer_stop(t);
}


