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
    /* malloc$B$7$F%P%C%U%!$r3NJ](B */
    buf = malloc(sizeof(VALUE)*10);
    ((VALUE*)buf)[0] = rb_hash_new();
    
    rb_define_global_function("hogefunc",hogefunc_c,0 );
}
