#include <iostream>
using namespace std;

int main()
{
	int *p = 0;
	
	try {
		*p=1;
	} catch(int i) {
		cout << "asdf" << endl;
	}
        
}
