#include <string>
#include <vector>
#include <iostream>


std::vector<std::string> splitString(std::string str, std::string delim)
{
    std::vector<std::string> result;
    size_t cutAt;
    while( (cutAt = str.find_first_of(delim)) != str.npos ){
        if(cutAt > 0){
            result.push_back(str.substr(0, cutAt));
        } else {
            result.push_back("");
        }
        str = str.substr(cutAt + 1);
    }
    
    if(str.length() > 0){
        result.push_back(str);
    }
    return result;
}
int main(int argc, char **argv ) {
    std::string s = "a,b,c,,,,,d,e,,,,,f,,g,,,,,,,h,i";
    std::vector<std::string>v = splitString( s, "," );
    for(int i=0;i<v.size();i++){
        std::cerr << "val " << i << " : '" << v[i] << "'" << std::endl;
    }
}
