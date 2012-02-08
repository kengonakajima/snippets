#include <vector>
#include <iostream>

int main() {
    std::vector<int>v;

    v.push_back(1);
    v.push_back(45);

    int x = v[ v.size()-1];
    v.pop_back();

    std::cerr << "n:" << v.size() << " x:" <<x <<std::endl;

    x = v[ v.size()-1];
    
    std::cerr << "n:" << v.size() << " x:" <<x <<std::endl;
}
