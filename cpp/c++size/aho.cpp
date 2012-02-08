#include <iostream>
using namespace std;

class A
{
        int a;
 public:
        int aaaaho(){}

};

template <class T>
class B
{
        T t;
        int a;
};



int main()
{
        cout << sizeof(A) << endl;
        B<int*> *b = new B<int*>();

        cout << sizeof(B<A>) << endl;
}
                
