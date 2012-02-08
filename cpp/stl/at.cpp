#include <vector>
#include <iostream>

using namespace std;

int main()
{

        vector<int> vi;
        vi.push_back(1);
        vi.push_back(2);
        vi.push_back(8);

        vector<int>::iterator vii;
        vii = vi.at(2);

        cout << *vii << endl;
}
