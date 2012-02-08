#include <stdio.h>
#include <string.h>

int main()
{
    char s[1000];
    char out[10000]="";
    for(int i=0;i<1000000;i++){
        snprintf(s, sizeof(s), "%d\n", i );
        if( ( i % 100000 ) == 0 ){
            strcat( out, s );
        }
    }
    printf("%s\n", out );
}
