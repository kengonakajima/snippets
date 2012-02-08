#include "json.h"

int main()
{
    clx::json hoge( "ringo : [1,2,3, { \"age\" : 100, \"boo\": \"poo\" } ] " );

    int aa[10] = {1,2,3,4,5,5,5,5,5,5};

    hoge["aho"] = 100;
    hoge["aho"] = "asdfasdf";

    std::cerr << "x:" << hoge.string().size() << std::endl;

}
