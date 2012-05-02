#include <iostream>

void *operator new(size_t size){
    printf("new:sz:%u\n",(unsigned int)size);
    return malloc(size);
}
void operator delete(void *pv){
    printf("freeing %p\n",pv);
    free(pv);
}

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
