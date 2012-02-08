#include <stdlib.h>


int main()
{
    char *a = (char*)malloc(100);
    printf("%p\n",a);
    a[0]=0;
    char *b = (char*)malloc(100);
    printf("%p\n",b);
    b[0]=0;    
    char *c = (char*)reallocf((void*)a,200);
    printf("%p\n",c);
    c[0]=0;

    char *d = (char*)malloc(130);
    printf("%p\n",d);
    
    return 0;
}
