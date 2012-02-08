#include "ruby.h"

char *buf;

static VALUE hogefunc_c()
{
    rb_gc_unregister_address( &( ((VALUE*)buf)[0]) );
    ((VALUE*)buf)[0] = rb_hash_new();
    rb_gc_register_address( &( ((VALUE*)buf)[0]) );
    return Qnil;
}

void Init_hoge()
{
    /* mallocしてバッファを確保 */
    buf = malloc(sizeof(VALUE)*10);
    ((VALUE*)buf)[0] = rb_hash_new();
    
    rb_define_global_function("hogefunc",hogefunc_c,0 );
}
