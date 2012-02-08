









int ringofunc(int a)
{
    printf("RRRRRR %d\n", a);
}


int aaaaa(int a)
{
    
    
    printf("aaaaa is called: %d\n" , a );
}



int main()
{
    void (*f)(int);
    
    printf("%d\n", ringofunc );
    printf("%d\n", aaaaa );

    f = 134514176;
    f( 100 );
    
    f = 134514144;
    f( 200 );

    


}
