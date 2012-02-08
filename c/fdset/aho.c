#include <sys/socket.h>
//#include <select.h>
#include <sys/select.h>


int main()
{

    printf( "%d,%d\n", FD_SETSIZE, INR_OPEN );
}
