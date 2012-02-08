#include <stdio.h>
class A
{
 public:    
    void aho(){printf("asdf\n");}
};

class B : public A
{
 public:
    void bho(){
        aho();
    }
    void bbho();
};
void B::bbho(){
    aho();
}

int main(){
    B *b = new B();
    b->aho();
    b->bho();
    b->bbho();    
}
