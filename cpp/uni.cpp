/*
  unicodeの空白文字を調べ上げる
  
 */

#include <iostream>
#include <string>

char aho[] = "aho\u3000hoge";



int main() {
    std::cerr << aho<<std::endl;

    std::string a(aho);

    int s = a.find( "\u3000" );
    if( s == std::string::npos ) std::cerr << "notfound"<<std::endl; else std::cerr << "found"<<std::endl;            

    s = a.find( "ho" );
    if( s == std::string::npos ) std::cerr << "notfound"<<std::endl; else std::cerr << "found"<<std::endl;        

    s = a.find( "\u2000" );
    if( s == std::string::npos ) std::cerr << "notfound"<<std::endl; else std::cerr << "found"<<std::endl;            
}
