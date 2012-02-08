


void (*a)(int a );

void register_func( void (* p)(int a ) )
{
    a = p;
}

int main()
{
    hoge_module_init();

    a(10);
}
