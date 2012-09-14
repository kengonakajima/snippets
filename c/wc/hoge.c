#include <stdio.h>

int main(){
    wchar_t hoge;
        
printf("size:%d\n", (int)sizeof(hoge));
    wchar_t s[5] = L"ABCD";
    printf("size:%d\n", (int)sizeof(s));
    char *v = (char*) s;
    for(int i=0;i<20;i++){
        printf("%d:%x\n", i, v[i] );
    }
    
}
