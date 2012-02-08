/*
  Ruby の拡張ライブラリのメモリリーク・テスト

  
 */

#include "ruby.h"

VALUE cAho;

struct aaa
{
    int k;
    VALUE a;
    VALUE a2;
};

static VALUE
hoge_func( VALUE obj , VALUE array )
{
    VALUE nv;
    struct aaa *aaap;
    VALUE a2;

    fprintf( stderr ,"aho\n" );

    a2 = rb_ary_new();
    nv = Data_Make_Struct( cAho, struct aaa, 0, free, aaap );

    aaap->a = array;
    aaap->a2 = a2;
    aaap->k = random();




    return nv;
}

void
Init_aho( void )
{
    cAho = rb_define_class( "Aho", rb_cObject );
    rb_define_method( cAho, "hoge", hoge_func, 1 );

}
