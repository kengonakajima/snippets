#include <iostream>

const char *aa[] = {
    "aho", "hoge"
};

void aho(const char *s ){
    std::cerr << strlen(s) << std::endl;
}

int main() {
    aho( "aho" );
}
