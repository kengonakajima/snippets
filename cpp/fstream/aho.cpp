#include <fstream>

int main(int argc, char **argv )
{
        std::fstream fs( "chatlog.txt", std::ios::out | std::ios::app );
        fs << "asdlfjalskdfjalskdjf";
        return 0;
}
