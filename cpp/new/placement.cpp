#include <stdlib.h>
#include <new>

class K {
public:
    int a,b;
    K(int i) {
        a=i;
        b=i*2;
    }
};
int main() {
    void *p=malloc(sizeof(K));
    K *k= new(p) K(1);
}
