
#include <iostream>
#include <string>
#include <sstream>

int main() {
        std::string n_string("0xff");
        std::istringstream n_stream(n_string);
        n_stream.unsetf(std::ios_base::dec);

        int result;
        n_stream >> result;
        std::cout << result << std::endl; // should output 255
}
