#include <stdio.h>

class X {
public:
    int n;
    void (*fptr)( X *x, int e );
};

void fff( X *x, int e ) {
    fprintf(stderr, "pppppppp:%d\n",x->n+e);
}
int main() {
    X *x = new X();
    x->fptr = fff;
    x->n = 10;
    x->fptr(x,2);
}
