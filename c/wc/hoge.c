#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>

int main(){
    wchar_t hoge;
    printf("sizeof wchar_t:%d\n", (int)sizeof(hoge));


    char *u8s = u8"あいう";
    int l = strlen(u8s);    
    wchar_t *out = (wchar_t*)malloc(1000);
    size_t wcl=mbstowcs(out, u8s, l+1 );

    printf("strlen of %s : %d\n",u8s,l);
    printf("wcl of %s : %d\n",u8s,(int)wcl);    
    for(int i=0;i<3;i++) {
        printf("[%d]:%x\n", i,out[i]);
    }



#if 0

        
    wchar_t s[5] = L"ABCD";
    printf("size:%d\n", (int)sizeof(s));
    char *v = (char*) s;
    for(int i=0;i<20;i++){
        printf("%d:%x\n", i, v[i] );
    }
    printf("Len:%d\n", wcslen(s) );
#endif
    
}
