#include <iostream>

class X
{
 public:
    int a,b;
    
    X(){
        a = 1;
        b = 2;
    }

    void aho(){
        if(this==NULL){
            std::cerr << "null" << std::endl;
        } else {
            b = 3;
            std::cerr << "aho" << std::endl;
        }
    }
};
int main() {

    X *x = new X();

    x->aho();

    delete x;

    x = NULL;
    x->aho();
}

