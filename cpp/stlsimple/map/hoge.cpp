#include <map>
#include <iostream>

class fuga
{
 public:
    int ppp[100000];
    int xxx;
    fuga(int x){xxx=x;}
    fuga(){}
};

int main()
{
    std::map<int,int>m;
    m[100100100] = 1;
    m[200200200] = 2;

    std::cerr << m[100100100] << std::endl;
    std::cerr << m[200200200] << std::endl;
    std::cerr << m[300300300] << std::endl;

    std::map<int,fuga*>f;
    f[100100100] = new fuga(1);
    f[200200200] = new fuga(2);
    std::cerr << f[100100100] << std::endl;
    std::cerr << f[200200200] << std::endl;
    std::cerr << f[300300300] << std::endl;

    std::map<int,fuga>g;
    g[100100100] = fuga(1);
    g[200200200] = fuga(2);
    std::cerr << g[100100100].xxx << std::endl;
    std::cerr << g[200200200].xxx << std::endl;
    std::cerr << g[300300300].xxx << std::endl;

    std::cerr << &g[100100100] << std::endl;
    std::cerr << &g[200200200] << std::endl;
    std::cerr << &g[300300300] << std::endl;        
    std::cerr << &g[400400400] << std::endl;
    std::cerr << &g[500500500] << std::endl;            
    std::cerr << &g[900900900] << std::endl;
    
    return 0;
}
