#include <vector>

typedef std::vector<int> iv;

std::vector<iv> g_iv;

int main()
{
    for(int i=0;i<2000000;i++){
        iv v;
                v.push_back(i);
        g_iv.push_back(v);
    }
    sleep(1000);
    
}
