#include <vector>
#include <string>
#include <iostream>

int convert(std::vector<int>& u, const std::string& s, bool& success) {
    success = false;
    int cp = 0;
    int runlen = 0;
    for (std::string::const_iterator it = s.begin(), end = s.end(); it != end; ++it) {
        int ch = static_cast<unsigned char>(*it);
        std::cerr << "ch:" << ch << " rl:" << runlen << std::endl;
        if (runlen > 0) {
            if ((ch & 0xc0 != 0x80) || cp == 0) {
                std::cerr << "A" << std::endl;
                return it-s.begin();
            }
            cp = (cp << 6) + (ch & 0x3f);
            if (--runlen == 0) {
                u.push_back(cp);
                cp = 0;
            }
        }
        else if (cp == 0) {
            if (ch < 0x80)      { u.push_back(ch); }
            else if (ch > 0xf8) {
                std::cerr << "B" << std::endl;                
                return it-s.begin();
            }
            else if (ch > 0xf0) { cp = ch & 7; runlen = 3; }
            else if (ch > 0xe0) { cp = ch & 0xf; runlen = 2; }
            else if (ch > 0xc0) { cp = ch & 0x1f; runlen = 1; }
            else {
                std::cerr << "C" << std::endl;                
                return it-s.begin(); // stop on error
            }
        }
        else {
            std::cerr << "D" << std::endl;            
            return it-s.begin();
        }
    }
    success = runlen == 0; // verify we are between codepoints
    return s.length();
}
int main( int argc, char **argv ) {
    char aho[] = "ABCあ"; // あいうえおABC";
    int r;
    std::vector<int>out;
    bool success=false;
    std::string hoge(aho);
    r = convert( out, hoge, success );
    std::cerr << "r:" << r << " success:" << success << std::endl;
    
}
