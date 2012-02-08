int aho( int a )
{
    return 1000000;
}

int main()
{
    int k;
    aho(100);
    __asm__( "push 1234");
    __asm__( "pop 1234");    
    k = aho(200);
    __asm__( "push 1234");
    __asm__( "pop 1234");    
    aho(100);
    __asm__( "push 1234");
    __asm__( "pop 1234");    
    k = aho(200);    
    return 0;
}
