#include <map>
#include <iostream>

int main()
{
    std::map<int,int> m;

    for(int i=0;i<1000000;i++){
        m[i]=i;
    }
    sleep(1000); 

}
