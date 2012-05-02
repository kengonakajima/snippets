#include <iostream>

#include "a.h"

class Hoge {
 public:
    int a;
    Hoge(int aho){
        a=aho;
        printf("hogecon:%d\n",aho);
    }
    ~Hoge(){
        printf("hogedel. aho:%d\n",this->a);
    }
};

int main(int argc, char **argv ) {
    Hoge *h = new Hoge(1234);
    delete h;
}
