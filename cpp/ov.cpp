#include <stdio.h>
#include <string>

class Y
{
 public:
    int x;

};

class X : public Y
{
 public:
    
    int x;
    
};

int main(){
    std::string a("aho");
    printf("%d\n", a.size());
}

