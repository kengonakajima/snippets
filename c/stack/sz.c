#include <stdio.h>

void analyze_stack(int value) {
    int stack_var;
    printf("Stack address before switch: %p\n", (void*)&stack_var);

    switch(value) {
        case 1: {
            int case1_var=1;
            printf("Case 1 stack address: %p v:%d\n", (void*)&case1_var, case1_var);
            break;
        }
        case 2: {
            int case2_var=2;
            printf("Case 2 stack address: %p v:%d\n", (void*)&case2_var,case2_var);
            break;
        }
        default: {
            int default_var=3;
            printf("Default case stack address: %p v:%d\n", (void*)&default_var, default_var);
        }
    }

    printf("Stack address after switch: %p\n", (void*)&stack_var);
}

int main() {
    analyze_stack(1);
    analyze_stack(2);
    analyze_stack(3);
    return 0;
}
