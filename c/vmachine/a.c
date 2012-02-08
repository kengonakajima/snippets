#include <stdio.h>

int main()
{
    int a =0;
    char line[100];
    while(fgets(line,sizeof(line),stdin)){
        a =a + atoi( line);
        printf("%d\n" , a );
    }
}
