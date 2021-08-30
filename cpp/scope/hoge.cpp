#include <inttypes.h>

int main() {
    uint16_t p=100;
    uint16_t k = (uint16_t)({ uint16_t _v = p; p*2; });

}
