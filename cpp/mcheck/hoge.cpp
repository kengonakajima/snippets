#include <mcheck.h>
#include <stdlib.h>

class K {
public:
    int a;
    int b;
    K() : a(0), b(0) {
        
    }
};

int main(){
    mtrace();  // start tracing
    K *k=new K();
    K *k2=new K();
    delete k2;
    char *p=(char*)malloc(100);
    muntrace(); // stop tracing
    // here, k and p is not freed
    return 0;
}
