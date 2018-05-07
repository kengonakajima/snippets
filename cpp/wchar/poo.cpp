#include <stdio.h>
#include <wchar.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "ConvertUTF.h"

int main(int argc, char **argv ) {
    const wchar_t *t0= L"ABC012あいうえお\nあいうえお(wchar_t)。";
    const char *t1= "ABC012あいうえお\nあいうえお(mb-utf8)。" ;


    const UTF32 *ut0=(UTF32*)t0;
    const UTF32 *ut1=(UTF32*)t1;    
    
    UTF8 *out=(UTF8*)malloc(1000);
    
    ConversionResult r=ConvertUTF32toUTF8 ( &ut0, ut0+wcslen(t0), &out, out+1000, strictConversion);
    printf("sizeof wchar_t:%d t0len:%d r:%d\n",sizeof(wchar_t), wcslen(t0),r );

    r=ConvertUTF32toUTF8 ( &ut1, ut1+strlen(t1), &out, out+1000, strictConversion); // error
    printf("sizeof wchar_t:%d t0len:%d r:%d\n",sizeof(wchar_t), strlen(t1),r );        
    
    return 0;
}
