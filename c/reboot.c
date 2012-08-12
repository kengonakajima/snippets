#include <unistd.h>
#include <sys/reboot.h>
int main() {
    int r =reboot(0);
    printf("%d\n",r);
}
