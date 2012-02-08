#include <netdb.h>
#include <unistd.h>

int
main()
{
    struct hostent *e;

    e = gethostbyname("drink");
    if(e){
        printf("%x\n", e );
    } else {
        printf("fuckcckck\n");
    }
}
