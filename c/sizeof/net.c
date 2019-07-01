#include <netinet/in.h>
#include <stdio.h>

int main() {
    printf("%lu %lu\n", sizeof(struct sockaddr_in), sizeof(struct sockaddr_in6)); // 16,28 in macos
}
