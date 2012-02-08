int main(int argc, char **argv )
{
    int start= 1;
    int i;
    
    for( start =1; start <= 1000; start ++ ){
        printf("--\n" );
        if( cal( start, atoi(argv[1]) ))break;
    }
}










int cal( int start, int count)
{
    int a , total;
    int i,found;

    found = 0;
    total = 0;
    a=start;
    for(i=0;i<count;i++){
        total += a;
        printf("A:%d total:%d\n", a, total );
        a*=1.5;
        if( total >= 1000 ){
            found = 1;
            break;
        }
    }
    if( found == 1 && i == count ){
        return 1;
    }
    if( found == 0 ) return 0;
    
}
