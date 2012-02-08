#include <string>
#include <iostream>

int main () {
    char aho[] = "akjsdflkajshdlkfjahsdlkfjhalskjdfh";
    std::string s ( aho, 4 );
    std::cerr << s << std::endl;
}
