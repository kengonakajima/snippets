#include <stdio.h>
int main(int argc, char **argv) {
    FILE *fp=fopen("abandon.pcm","rb");
    static short buf[1024*1024];
    int nr=fread(buf,2,sizeof(buf)/2,fp);
    printf("NR:%d\n",nr);
    for(int i=0;i<nr;i++) {
        printf("%d\n", buf[i]);
    }
    return 0;
}
