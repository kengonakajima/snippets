#include <stdio.h>

int main() {
    char l[1024];
    int n=0;
    while(fgets(l,sizeof(l),stdin)){
        // 8spaces       <node id="700079" lat="17.0870471" lon="120.9085202" version="6" timestamp="2016-01-16T09:37:16Z" changeset="0"/>
        for(int i=0;i<20;i++) {
            if(i<10&&l[i]=='<' && l[i+1]=='n' && l[i+2] == 'o' && l[i+3]== 'd') {
                n++;
                if(n%1000000==0) printf("d:%d\n",n);
            }
        }
    }
}
