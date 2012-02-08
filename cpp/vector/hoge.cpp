#include <vector>
using namespace std;

int main(){
        vector<int> v;

        v.push_back(10);
        v.push_back(20);
        v.push_back(30);
        v.erase(v.begin());
        printf("%d\n", v[0] );
        printf("%d\n", v.size() );
}
