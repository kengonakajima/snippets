// sample.c
#include <stdio.h>

int add(int a, int b) {
    return a + b;
}


void print_shorts(short *arr, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


