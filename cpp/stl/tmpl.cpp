#include <iostream>
using namespace std;

template <typename T>
class MyV {
	public:
	private:
	T* data;
};

template <typename T>
void ringoswap(T& a, T& b )
{
	T tmp = a; a = b; b = tmp;
}

int main()
{
	int x=10,y=20;
	ringoswap( x, y );
	
	cout << "x:" << x << " y:" << y << endl;
	
	return 0;

}