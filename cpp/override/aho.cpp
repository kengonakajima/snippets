#include <iostream>

using namespace std;

class Basic
{
	public:
	int a;
	Basic() { a = 0; };
	virtual  ~Basic( ){}
	virtual void foo() = 0; // pure virtual function
	
};

class Sub : public Basic
{
	public:
	// overriding
	void foo() {
		cout << "Im sub." << endl;
		
	}
	void foo( int i_in )
	{
		cout << "Im sub.foo(int). in:" << i_in <<  endl;

	}
	int foo( int i_in, char *szbuf ){
		cout << "Im sub foo(int). in:" << i_in << szbuf << endl;
		return 1;
	}

	
};

class Sub2 : public Basic
{
	public:
	// overriding
	void foo(){
		cout << "Im sub2" << endl;
	}
};

int main()
{
	Basic *pB = new Sub;
	Basic *pB2 = new Sub2;
	
	Sub *pS = new Sub;
	
	pB->foo();
	
	pS->foo();
	pS->foo(88);
	pS->foo(90, "Hoge" );
	
	pB2->foo();

	delete pB;
	delete pB2;
	delete pS;
	
	return 0;
}