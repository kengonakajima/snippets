// sample.c
#include <stdio.h>
#define EXPORT __declspec(dllexport)

EXPORT int add(int a, int b) {
    return a + b;
}


EXPORT void print_shorts(short *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


