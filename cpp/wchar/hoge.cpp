
#include <stdio.h>
#include <wchar.h>

int main() {
    wchar_t aho[] = L"ほ";
    printf("sz:%d l:%d\n", sizeof(wchar_t), wcslen(aho) );
}
