#include <string>
#include <algorithm>
#include <iostream>

int main(){

        std::string s;
        s = "askdf";

        std::wstring ws(s.length(), L' ');
        std::copy(s.begin(), s.end(),  ws.begin());
        printf("%s\n", s.c_str());
        printf("%s\n", ws.c_str());
}
