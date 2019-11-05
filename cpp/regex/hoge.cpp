#include <regex>
#include <iostream>

int main() {
    const char *input="   <tag k=\"avsdf:kasdf\" v=\"ほげほげほadfasdf:ak=v=df.asdf&quot;\"/>";
    std::string s(input);
    std::regex kr("k=\"([a-z:_]+)\"");
    std::regex vr("v=\"(.+)\"");
    auto kbeg=std::sregex_iterator(s.begin(),s.end(),kr);
    auto end=std::sregex_iterator();
    auto vbeg=std::sregex_iterator(s.begin(),s.end(),vr);
    for(std::sregex_iterator i=kbeg; i!=end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        if (match_str.size() > 0 ) std::cout << "  " << match_str << '\n';
    }
    for(std::sregex_iterator i=vbeg; i!=end; ++i) {
        std::smatch match = *i;
        std::string match_str = match.str();
        if (match_str.size() > 0 ) std::cout << "  " << match_str << '\n';
    }
    
}
