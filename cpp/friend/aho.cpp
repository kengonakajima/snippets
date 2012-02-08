#include <iostream>

class Helper;

class Server
{
    friend class Helper;
    
 public:
    int m_a,m_b;
    Server() : m_a(0), m_b(0) { }
    
};

class Helper
{
 public:
    void helpSendFunc( int xxx ){
        m_b = xxx;
    }
};

int main()
{
    Server *s = new Server();

    s->helpSendFunc( 100);

    std::cerr << "m_b:" << s->m_b << std::endl;
}


