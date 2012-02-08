static int
ITEM_randRange( int base, double min_rate , double max_rate )
{
    int min = base * min_rate;
    int max = base * max_rate;
    int range = ( max - min );

    if( range == 0 ) return range;
    if( range < 0 ) return 0;

    return min + random() % range;
}

int main()
{
    printf( "%d\n", ITEM_randRange( 100, 0.7, 1.4 ) );
    printf( "%d\n", ITEM_randRange( 100, 0.7, 1.4 ) );
    printf( "%d\n", ITEM_randRange( 100, 0.7, 1.4 ) );

    printf( "%d\n", ITEM_randRange( 100, 0.7, 1.4 ) );
    printf( "%d\n", ITEM_randRange( 100, 0.7, 1.4 ) );    
    
}
