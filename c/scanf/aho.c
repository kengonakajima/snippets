#include <stdio.h>
int main() {
    char aho[100];
    sprintf(aho, "aho/a_1_-2_-3.poo");
    int x=-9999,y=-9999,z=-9999;
    sscanf(aho, "aho/a_%d_%d_%d.poo",&x,&y,&z );
    printf("aaaa:%d,%d,%d",x,y,z);
}
