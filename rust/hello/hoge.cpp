#include <stdio.h>
#include <string.h>
#include <stdio.h>
int main() {
    FILE *fp=fopen("hoge_c.txt","w");
    for(int i=0;i<1000000;i++) {
        char s[100];
        snprintf(s,sizeof(s),"hoge:%d\n",i);
        fwrite(s,1,strlen(s),fp);
        //        fflush(fp);
    }
    fclose(fp);
    return 0;
}
