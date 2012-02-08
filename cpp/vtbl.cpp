#include <stdio.h>

class V
{
    int a;
    //    virtual void aho(){}
};

class VV  : public V
{
    int b;
    virtual void aho(){}
};

int main()
{
    V *v = new V();
    VV *vv = new VV();

    printf("%d\n", sizeof(*v));
    printf("%d\n", sizeof(*vv));    
}
