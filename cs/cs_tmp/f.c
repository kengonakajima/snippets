#include <stdio.h>

int main() {
    FILE *fp=fopen("hoge_c.txt","w");
    for(int i=0;i<1000000;i++) {
        fprintf(fp,"hoge:%d",i);
    }
    fclose(fp);
}
