#include <stdio.h>
#include <stdint.h>

struct poo {
    uint64_t hoge : 40;    
};


int main() {
    printf("%lu\n",sizeof(poo));
}
