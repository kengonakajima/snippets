#include <pwd.h>
#include <sys/types.h>0
#include <stdio.h>

int main()
{
    int i;

    for(i=0;i<100000;i++){
        struct passwd *p;
        p = getpwnam( "ringo" );
        /*   printf( "hoge: [%s] [%s] u[%d] g[%d] gecos[%s] dir[%s] sh[%s]\n",
            p->pw_name, p->pw_passwd ,
            p->pw_uid , p->pw_gid , p->pw_gecos,
            p->pw_dir , p->pw_shell );*/
    }
}
