#include <stdio.h>

void poo(int a) {
    switch(a) {
    case 0:
        {
            char aho[1024][1024];
            printf("aho:%d %p\n",a, aho);
        }
        break;
    case 1:
        {
            char aho[1024][1024];
            printf("aho:%d %p\n",a, aho);
        }
        break;
    case 2:
        {
            char ahoa[1024][1024];
            printf("aho:%d %p\n",a, ahoa);
        }
        break;
    default:
        {
            char hoga[1024][1024];
            char hah[1024][1024];
            printf("def:%d %p\n",a,hoga);
        }
        break;
    }
}

int main() {
    for(int i=0;i<10;i++) {
        poo(i);
    }
}
