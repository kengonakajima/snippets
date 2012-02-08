#include <iostream>

class Aho
{
public:
	Aho()
	{
		
	}
};

template <class T> class ahoaho  : public Aho
{
	T *x;
	public:	
	ahoaho()
	{
		x = new T();
	}
};

class hogehoge : public ahoaho<Aho>
{
	public:
	hogehoge()
	{
		
	}
};

int main()
{
	hogehoge *h = new hogehoge();
	
}