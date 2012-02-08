struct aho
{
    int a[10];
    int b[10];
    int c;
};


int main( )
{
    struct aho * a = 0;


    printf( "%p , %p \n", a, a->a );
    printf( "%p , %p \n", a, a->b );
    printf( "%p , %p \n", a, & a->c );
    printf( "%p , %p \n", a, a->c );        


}
