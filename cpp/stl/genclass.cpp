#include <iostream>
using namespace std;

template <typename T, typename U >
class X {
	T& v1;
	U& v2;
	public:
	X(T &a, U &b){
		v1 = a;
		v2 = b;
	}
	T square( T t ){ return t * t ; }
};

int main()
{

//ERROR	
//	int m = 66;
//	X<short,m> x3;

	X<float,int> xxx( 100.0f, 200 );
	cout << xxx.square( 11 ) << endl;
	return 0;
}