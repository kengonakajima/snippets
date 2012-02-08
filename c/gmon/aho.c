#include <stdio.h>

long long j = 0;


void hoge(int n)
{
        int i;
        for(i=0;i<n*10000;i++){
                j += i;
        }
}
int main()
{
        int i;
        for(i=0;i<500;i++){
                hoge(i);
        }
        printf("%lld\n", j );
}
