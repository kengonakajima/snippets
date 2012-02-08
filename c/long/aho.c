long long i;
__int64 iii;

int main()
{
    for(i=0;;i++){
        if((i%10000000)!=0) {
            continue;
        }
        
        printf( "%lld\n",i);
    }
}
