#include <stdio.h>

static void func_in_hoge ( int b )
{

    printf( "hoged:%d\n",b  );
    
}
void hoge_module_init( void )
{
    register_func( func_in_hoge );
}

