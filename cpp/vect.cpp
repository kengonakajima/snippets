#include <vector>
#include <iostream>

class A
{
 public:
    int iiii;
    
    A(int a) {
        iiii=a;
    }
    ~A() {
        iiii=0;
    }
    
    
};

int main() {
    std::vector< A*> v;
    v.push_back( new A(1));
    v.push_back( new A(3));
    v.push_back( new A(8));

    std::vector< A*>::iterator it;
    std::vector< A*> vv;
    vv = v;
    
    for(it=vv.begin(); it !=vv.end(); ++it ){
        A* ap = (*it);
        if(ap->iiii==3){
            delete ap;
            (*it)=NULL;
        }
    }
    for(it=v.begin(); it !=v.end(); ++it ){
        printf("%x\n", (*it));
    }
}
