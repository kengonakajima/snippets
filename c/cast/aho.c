struct lvm_big
{
    int len;
    char data[1024];
};

struct lvm_small
{
    int len;
    char data[64];
};

struct lvm_value
{
    int len;
    char data[1];
};

int main()
{
    init_lvm({{1024, 10}, {64,100}});

    struct lvm_small s;


    method_call( (struct lvm_value*) &s );

}

void method_call( struct lvm_value *v )
{
    memset( v, 0, v.len );
}



