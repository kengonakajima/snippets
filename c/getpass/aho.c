#include <stdio.h>
#include <unistd.h>
#include <pwd.h>


int main()
{
    char a[111];
    char *p;
    strcpy(a , "���ۤϤ���:");
    p = getpass(a);

    printf( "%s\n", p );
    
}
