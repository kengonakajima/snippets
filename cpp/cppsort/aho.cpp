#include <vector>
#include <algorithm>
#include <iostream>


class X
{
 public:
        X(int hoge ){ v = hoge; }
        int v;
};

bool operator<(const X&a, const X&b)
{
        return a.v < b.v ;
}

int main()
{
        X x1(10);
        X x2(1);
        X x3(5);
        std::vector<X>v;
        v.push_back(x1);
        v.push_back(x2);
        v.push_back(x3);

        std::sort(v.begin(), v.end());


        std::cout << v[0].v << "," << v[1].v << "," << v[2].v << std::endl;
}
