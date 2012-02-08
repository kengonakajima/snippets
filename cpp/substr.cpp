#include <string>
#include <iostream>

int main()
{
	using namespace std;

	string str( "abcdefghijk" );
	string::size_type index = str.find( "fgh" );  // "fgh"を検索
	if( index == string::npos )  // 検索できたかどうか
	{
		cout << "検索に失敗しました" << endl;
	}
	else
	{
		cout << str.substr( index ) << endl;
	}

	return 0;
}

