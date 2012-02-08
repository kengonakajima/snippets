#include <iostream>
#include <typeinfo>

using namespace std;

class foo {
	int  i;
	// ...
};

class ping {
	foo a;
};
class pong {
	ping a;
};

int main( int argc, char **argv )
{
	char	c;
	char*	cp;
	int		i;
	float	f;
	foo		ob,*pob;
	ping    q;
	pong 	qq;
	
	cout << "type of   c: " << typeid( c ).name( ) << endl;
	cout << "type of  cp: " << typeid( cp ).name( ) << endl;
	cout << "type of   i: " << typeid( i ).name( ) << endl;
	cout << "type of   f: " << typeid( f ).name( ) << endl;
	cout << "type of  ob: " << typeid( ob ).name( ) << endl;
	cout << "type of pob: " << typeid( pob ).name( ) << endl;
	cout << "type of pob: " << typeid( pob ).name( ) << endl;
	cout << "type of q: " << typeid( q ).name( ) << endl;	
	cout << "type of qq: " << typeid( qq ).name( ) << endl;		
	
	return 0;
}
