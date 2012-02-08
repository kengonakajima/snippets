#include <vector>
#include <iostream>

using namespace std;

void hoge( vector<int>::iterator i, vector<int>::iterator j )
{
	vector<int>::iterator k;
	for( k = i; k < j; k++ ){
		cout << *k << endl;
	}
}
	
	
int main()
{
	vector<int> iv;
	
	
	iv.push_back(10);
	iv.push_back(20);
	iv.push_back(30);
	iv.push_back(40);
	
	vector<int>::iterator i = iv.begin();
	vector<int>::iterator j = iv.end();	
	
	hoge( i,j );

}