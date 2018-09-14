#include <sys/utsname.h>
#include <stdio.h>

int main() {
    struct utsname u;
    int r=uname(&u)    ;
    printf("r:%d name:%s",r,u.sysname);
}
