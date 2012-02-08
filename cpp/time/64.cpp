#include <iostream>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>



unsigned long long GetCurrentTime()
{
	struct timeval tmv;
	gettimeofday( &tmv, NULL );
    std::cerr << "sec:" << tmv.tv_sec << "," << tmv.tv_usec << std::endl;
	static const unsigned long long  startup=(unsigned long long)tmv.tv_sec*1000000+tmv.tv_usec;
    std::cerr << "startup:" << startup << std::endl;
	unsigned long long now=(unsigned long long )tmv.tv_sec*1000000;
    std::cerr << "now:" << now << std::endl;
	now+=tmv.tv_usec;
    std::cerr << "startup:" << startup << std::endl;
    
	return (now-startup)/1000;
}

int main()
{

    
    unsigned long long x = GetCurrentTime();
    std::cerr << "x:" << x << std::endl;

    unsigned long long s = 50LL * 10000LL * 10000LL;
    unsigned long long e = 60LL * 10000LL * 10000LL;
    std::cerr << "s:" << s << std::endl;
    unsigned long t = (unsigned long)( e - s );
    std::cerr << "t:" << t << std::endl;
}

