#include <stdio.h>

int main()
{

    int c=0;
    int ary[100];
    
    while(1){
        ary[c]=c;
        
        c++;
        fprintf(stderr, "%d\n", c );
        usleep(100*1000);
    }
    /*
    int a=0;
    int *b;
    b = &a;
    
    printf("hoge\n");

    *b = 1;

    printf("fuga\n");
    */
}
