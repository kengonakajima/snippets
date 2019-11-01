#include <stdio.h>
int main() {
    char buf[4];
    FILE*fp=fopen("hoge.c","r");
    char *p=fgets(buf,4,fp);
    if(p) fprintf(stderr,"%c %c %c %c", p[0], p[1],p[2],p[3]);
}
