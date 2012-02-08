#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;


void dump( vector<int> v )
{
	for( vector<int>::iterator i = v.begin(); i != v.end(); i++ ){
		cout << *i << endl;
	}
}
void sdump( vector<string> v )
{
	for( vector<string>::iterator i = v.begin(); i != v.end(); i++ ){
		cout << *i << endl;
	}
}

bool gop( const int &a, const int &b )
{
	return a > b;
}

int main()
{
	vector<int> iv;
	cout << "v.sz:" << iv.size() << endl;
	iv.push_back(10);
	iv.push_back(9);
	iv.push_back(8);	
	cout << "v.sz:" << iv.size() << endl;
	for(int i=0;i<3;i++){
		cout << iv[i] << endl;
	}
	
	vector<int>::iterator ii = iv.begin();
	iv.insert(ii, 111 );
	iv.resize(10, 100 );
	int x;
	iv.pop_back();
	cout << "v.sz:" << iv.size() << endl;
	dump( iv );
	
	cout << "v.sz:" << iv.size() << endl;	

	vector<int>::iterator first = iv.begin();
	vector<int>::iterator last = iv.end();
	vector<int>::iterator iret = find(first, last, 111 );
	if( iret != last ){
		cout << "found!" << endl;
	} else {
		cout << "not found!" << endl;
	}

	cout << "--------------------" << endl;

	sort(first, last, greater<int>()  );
	dump(iv);
	

	
	iv.clear();
	cout << "v.sz:" << iv.size() << endl;
		
	vector<string> sv;
	sv.push_back( "aho" );
	sv.push_back( "hoge");
	sv.push_back( "nasu");
	sv.push_back( "camoo");
	
	sort( sv.begin(), sv.end() );
	
	sdump(sv);
	
	return 0;
}