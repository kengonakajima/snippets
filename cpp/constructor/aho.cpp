#include <iostream>

using namespace std;

class Foo
{
	private:
	int a;
	
	public:
	Foo( int x = 99 ){
		cout << "Im constructor" ;
		a = x;
		cout << a << endl;
	}
	virtual ~Foo() { ; }
};

int main()
{
	Foo *p = new Foo[3];
	
	if( p == NULL ){
		cout << "bad_alloc" << endl;
	} else {
		cout << "success!" << endl;
	}
	return 0;
}