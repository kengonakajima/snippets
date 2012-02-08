int main()
{

    int i;
    unsigned short s = 10000;
    char *a;
    

    i = 4;      /* 0x 00 00 00 04 --> 0x 04 00 00 00 */


    a = (char*) &s ;

    printf("%d %d\n" , a[0] , a[1] );


    a[0] = 0x39;
    a[1] = 0x16;

    printf("%d\n" , s );
    
   
    
}
