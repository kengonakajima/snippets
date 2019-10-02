#include <stdio.h>
#include <limits.h>
#include <stdint.h>

int main() {
    printf("ullmax:%llu\n",ULLONG_MAX);
    printf("llmax:%lld\n",LLONG_MAX);
    printf("llmin:%lld\n",LLONG_MIN);
    printf("ulmax:%u",UINT64_MAX);
}
