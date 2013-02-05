#include <stdio.h>
#include <stdlib.h>

//inline void * operator new ( size_t size, const char *file, int line ) {
inline void * operator new ( size_t size ) {    
    printf("new called. size:%d\n", (int)size );
    return malloc(size);
}
class A {
public:
    int a,b,c;
    A(){
        a=b=c=18;
    }
};




int main() {
    A *a = new A();

    printf("%d\n", a->b );
}
