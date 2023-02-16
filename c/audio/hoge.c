#include <stdio.h>
int main() {
    const int n=24000*30;
    short data[n];
    for(int i=0;i<n;i++) {
        data[i]=0;
        if(i%48000==5000)data[i]=10000;
        else if(i%48000==5001)data[i]=-10000;
    }
    FILE *fp=fopen("click.s16le","w");
    fwrite(data,1,sizeof(data),fp);
    fclose(fp);
}
