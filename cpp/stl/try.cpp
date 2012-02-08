#include <iostream>
using namespace std;

void sub()
{
	throw static_cast<string>("asdf");
}

int main()
{
	cout << "start" << endl;
	
	try{
		cout << "inside" << endl;
		sub();
		cout << "noreach" << endl;
	} catch(int i){
		cout << "int" << i << endl;
	} catch( string s) {
		cout << "str" << s << endl;
	} catch( ... ){
		cout << "default" << endl;
	}
	cout << "end" << endl;
	return 0;	
}