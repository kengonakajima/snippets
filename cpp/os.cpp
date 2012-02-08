#include <iostream>
#include <string>


int main(){
    std::ostringstream os;
    bool a = true;
    os <<  a;
    std::cerr << os.str() << std::endl;
}
