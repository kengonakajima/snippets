
#define N  10000000
#define DN 10000000.0


int
main()
{
    int i;
    double d = 0.0;
    
    for(i=0;i<1000000;i++){
        d += 1;
    }

    printf( "%f %f %f\n", 1000000.0, d, 1.0 * 1000000.0 );
    printf( "%d\n", d == 1000000 );
}
