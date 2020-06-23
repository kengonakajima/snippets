#include <iostream>
#include <execinfo.h>
#include <cxxabi.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <memory>

int main() {
    int callstack[128]={};
    int frames = backtrace((void**)callstack,128);
    printf("frames:%d\n",frames);
    char **strs = backtrace_symbols((void**)callstack,frames);
    for(int i=0;i<frames;i++) {
        printf("[%d] %s\n", i, strs[i]);
    }

    return 0;
}
