
#include <msgpack.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

            
double getTime(){
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return tv.tv_usec/1000000.0 + tv.tv_sec*1.0;
}


void strtest( char *buf, size_t sz ){
    msgpack_sbuffer* buffer = msgpack_sbuffer_new();
    msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);

    msgpack_pack_raw(pk,sz);
    msgpack_pack_raw_body(pk,buf,sz);
    size_t pklen = buffer->size;
    assert(pklen>sz);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    bool suc = msgpack_unpack_next(&msg,buffer->data, buffer->size, NULL);
    assert(suc);
        
    msgpack_object obj = msg.data;
    assert(obj.type == MSGPACK_OBJECT_RAW);
    assert(obj.via.raw.ptr[0]=='Z');
    assert(obj.via.raw.ptr[sz-1]=='Z');    
    
    msgpack_sbuffer_free(buffer);
    msgpack_packer_free(pk);
    msgpack_unpacked_destroy(&msg);
}

void numbertest( double *buf, size_t sz ) {
    msgpack_sbuffer* buffer = msgpack_sbuffer_new();
    msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);
    int i;
    msgpack_pack_array(pk,sz);
    for(i=0;i<sz;i++){
        msgpack_pack_int(pk, (int)buf[i] );
    }
    size_t pklen = buffer->size;
    assert(pklen>sz);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    bool suc = msgpack_unpack_next(&msg,buffer->data, buffer->size, NULL);
    assert(suc);

    msgpack_object obj = msg.data;
    assert(obj.type == MSGPACK_OBJECT_ARRAY);
    assert(obj.via.raw.ptr[0]);
    msgpack_object msg0 = obj.via.array.ptr[0];
    msgpack_object msgend =obj.via.array.ptr[sz-1];    
    assert(msg0.via.i64==0);
    assert(msgend.via.i64==(sz-1));

#ifdef WITH_MALLOC_COMPARISON
    double **bufbuf =malloc(sizeof(double*)*obj.via.array.size);
    for(i=0;i<obj.via.array.size;i++){
        bufbuf[i] = malloc(sizeof(double));
    }
    for(i=0;i<obj.via.array.size;i++){
        free(bufbuf[i]);
    }
    free(bufbuf);
#endif    
        
            
    msgpack_sbuffer_free(buffer);
    msgpack_packer_free(pk);
    msgpack_unpacked_destroy(&msg);    
}

void str_speedtest(size_t sz, int nloop) {
    double st = getTime();
    char *mem=malloc(sz);
    memset(mem,'Z',sz);
    
    int i;
    for(i=0;i<nloop;i++){
        strtest(mem,sz);
    }
    double et = getTime();    
    fprintf(stderr,"str_speedtest: sz:%d loop:%d %f sec. %d(q/sec)\n", (int)sz, nloop, (et-st), (int)(nloop/(et-st)));
    free(mem);
}

void number_speedtest(size_t sz, int nloop ) {
    double st = getTime();
    int i;
    double *mem=malloc(sizeof(double)*sz);
    for(i=0;i<sz;i++){
        mem[i] = (double)i;
    }

    for(i=0;i<nloop;i++){
        numbertest(mem,sz);
    }
    double et = getTime();    
    fprintf(stderr,"int_speedtest: sz:%d loop:%d %f sec. %d(q/sec)\n", (int)sz, nloop, (et-st), (int)(nloop/(et-st)));
    free(mem);    
}

/* take unfinished buffer without stream*/
void unfinished_nostream( void ) {
    /* creates buffer and serializer instance. */
    msgpack_sbuffer* buffer = msgpack_sbuffer_new();
    msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);
 
    /* serializes ["Hello", "MessagePack"]. */
    msgpack_pack_array(pk, 2);
    msgpack_pack_raw(pk, 5);
    msgpack_pack_raw_body(pk, "Hello", 5);
    msgpack_pack_raw(pk, 11);
    msgpack_pack_raw_body(pk, "MessagePack", 11);

    size_t origlen = buffer->size; // 19
    
    /* deserializes it. */
    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    bool success = msgpack_unpack_next(&msg, buffer->data, origlen-4, NULL);
    fprintf(stderr, "unfinished_nostream: short data should fail:%d\n", success);
    success = msgpack_unpack_next(&msg, buffer->data, origlen, NULL);
    fprintf(stderr, "unfinished_nostream: again: success?:%d\n", success);
    
 
    /* prints the deserialized object. */
    fprintf(stderr, "data:");
    msgpack_object obj = msg.data;
    msgpack_object_print(stderr, obj);  /*=> ["Hello", "MessagePack"] */
 
    /* cleaning */
    msgpack_sbuffer_free(buffer);
    msgpack_packer_free(pk);

    
}


int withstream(void) {
    /* serializes multiple objects using msgpack_packer. */
    msgpack_sbuffer* buffer = msgpack_sbuffer_new();
    msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);
    msgpack_pack_int(pk, 1);
    msgpack_pack_int(pk, 2);
    msgpack_pack_int(pk, 3);
 
    /* deserializes these objects using msgpack_unpacker. */
    msgpack_unpacker pac;
    msgpack_unpacker_init(&pac, MSGPACK_UNPACKER_INIT_BUFFER_SIZE);
 
    /* feeds the buffer. */
    msgpack_unpacker_reserve_buffer(&pac, buffer->size);
    memcpy(msgpack_unpacker_buffer(&pac), buffer->data, buffer->size);
    msgpack_unpacker_buffer_consumed(&pac, buffer->size);
 
    /* now starts streaming deserialization. */
    msgpack_unpacked result;
    msgpack_unpacked_init(&result);
 
    while(msgpack_unpacker_next(&pac, &result)) {
        msgpack_object_print(stdout, result.data);
        puts("");
    }
 
    /* results:
     * $ gcc stream.cc -lmsgpack -o stream
     * $ ./stream
     * 1
     * 2
     * 3
     */
}

int withloop(void) {
 
    /* creates buffer and serializer instance. */
    msgpack_sbuffer* buffer = msgpack_sbuffer_new();
    msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);
        
    int j;
 
    for(j = 0; j<23; j++) {
        /* NB: the buffer needs to be cleared on each iteration */
        msgpack_sbuffer_clear(buffer);
 
        /* serializes ["Hello", "MessagePack"]. */
        msgpack_pack_array(pk, 3);
        msgpack_pack_raw(pk, 5);
        msgpack_pack_raw_body(pk, "Hello", 5);
        msgpack_pack_raw(pk, 11);
        msgpack_pack_raw_body(pk, "MessagePack", 11);
        msgpack_pack_int(pk, j);
 
        /* deserializes it. */
        msgpack_unpacked msg;
        msgpack_unpacked_init(&msg);
        bool success = msgpack_unpack_next(&msg, buffer->data, buffer->size, NULL);
 
        /* prints the deserialized object. */
        msgpack_object obj = msg.data;
        msgpack_object_print(stderr, obj);  /*=> ["Hello", "MessagePack"] */
        puts("");
    }
 
    /* cleaning */
    msgpack_sbuffer_free(buffer);
    msgpack_packer_free(pk);
}

void hello(void){
    /* creates buffer and serializer instance. */
    msgpack_sbuffer* buffer = msgpack_sbuffer_new();
    msgpack_packer* pk = msgpack_packer_new(buffer, msgpack_sbuffer_write);
 
    /* serializes ["Hello", "MessagePack"]. */
    msgpack_pack_array(pk, 2);
    msgpack_pack_raw(pk, 5);
    msgpack_pack_raw_body(pk, "Hello", 5);
    msgpack_pack_raw(pk, 11);
    msgpack_pack_raw_body(pk, "MessagePack", 11);

    fprintf(stderr, "buffer size:%d\n", (int)buffer->size );
    
    /* deserializes it. */
    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    bool success = msgpack_unpack_next(&msg, buffer->data, buffer->size, NULL);
 
    /* prints the deserialized object. */
    fprintf(stderr, "data:");    
    msgpack_object obj = msg.data;
    msgpack_object_print(stderr, obj);  /*=> ["Hello", "MessagePack"] */
 
    /* cleaning */
    msgpack_sbuffer_free(buffer);
    msgpack_packer_free(pk);
}



int main(void) {
    hello();
    withloop();
    withstream();
    unfinished_nostream();
    fprintf(stderr,"\n");
    number_speedtest(1,100*1000);
    number_speedtest(10,100*1000);
    number_speedtest(100,100*1000);
    number_speedtest(1000,10*1000);
    number_speedtest(10000,10*1000);            
    str_speedtest(1,100*10000);
    str_speedtest(10,100*10000);    
    str_speedtest(100,100*10000);
    str_speedtest(1000,100*10000);
    str_speedtest(10000,100*10000);
    return 0;
}
