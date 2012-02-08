#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

string escaper( string src )
{
        int len = src.length();
        char *p = new char[len*2+1];
        memcpy( p, src.c_str(), len );
        memcpy( p+len, src.c_str(), len );
        p[len*2]=0;
        src.reserve( len*2+1 );
        src.assign(p);
        free(p);
        return src;
}

int main( int argc , char *argv )
{
        int i;

        string a = "aksdjhflakjsdhf";
        for(i=0;i<10000000;i++){
                string hoge = escaper( a );
                usleep(100);
                cout << i << endl;
        }

        return 0;
}
