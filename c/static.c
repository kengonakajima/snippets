#include <stdio.h>

char buf[1000*1024];

int main()
{
    printf("as:%d\n", buf[ sizeof(buf)-10 ]);

}
