
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
 
using namespace std;
 
int lower_case ( int c )
{
  return tolower ( c );
}
 
int main()
{
  string s = "THIS IS A TEST";
 
  transform ( s.begin(), s.end(), s.begin(), lower_case );
  cout<< s <<endl;
}

/*

#include <string.h>

int main()
{
    char *s = strlwr( "aHo" );

    printf("%s\n", s );
}
*/
