#include <tr1/unordered_map>
#include <iostream>
#include <map>

/*<title> timevaldiff - get difference of two timeval in msec </title>*/
#include <sys/time.h>

/*      struct timeval {
               long tv_sec;         seconds 
               long tv_usec;   microseconds 
       };
*/

long timevaldiff(struct timeval *starttime, struct timeval *finishtime)
{
  long msec;
  msec=(finishtime->tv_sec-starttime->tv_sec)*1000;
  msec+=(finishtime->tv_usec-starttime->tv_usec)/1000;
  return msec;
}

#ifdef TEST
#include <stdio.h>
#include <stdlib.h>

int main() {

  struct timeval start, finish;
  long msec;

  gettimeofday(&start, NULL);
  sleep(1);
  gettimeofday(&finish, NULL);
  msec = timevaldiff(&start, &finish);
  printf("Elapsed time for sleep(1) is: %d milliseconds.\n", msec);
  return 0;
}
#endif


int main()
{
    struct timeval s,f;

    //1
    std::tr1::unordered_map< int, int> m;
    gettimeofday( &s, NULL );
    int n = 1000000,tot;
    for(int i=0;i<n;i++){
        m[i]=i*10;
    }
    gettimeofday( &f, NULL );
    std::cerr << "unom-create:"<<timevaldiff( &s, &f) << std::endl;
    // 1-1
    gettimeofday( &s, NULL );
    tot=0;
    for(int i=0;i<n;i++){
        tot += m[i];
    }
    gettimeofday( &f, NULL );
    std::cerr << "unom-read:"<<timevaldiff( &s, &f) << std::endl;
    // 1-2
    gettimeofday( &s, NULL );
    for(int i=0;i<n;i++){
        m.erase(i);
    }
    gettimeofday( &f, NULL );
    std::cerr << "unom-erase:"<<timevaldiff( &s, &f) << " sz:" << m.size() << std::endl;
    //2
    gettimeofday( &s, NULL );
    std::map<int,int>mm;
    for(int i=0;i<n;i++){
        mm[i]=i*10;
    }
    gettimeofday( &f, NULL );
    std::cerr << "stdm-create:"<<timevaldiff( &s, &f) << std::endl;
    //2-1
    gettimeofday( &s, NULL );
    tot=0;
    for(int i=0;i<n;i++){
        tot += mm[i];
    }
    gettimeofday( &f, NULL );        
    std::cerr << "stdm-read:"<<timevaldiff( &s, &f) << std::endl;
    // 1-2
    gettimeofday( &s, NULL );
    for(int i=0;i<n;i++){
        mm.erase(i);
    }
    gettimeofday( &f, NULL );
    std::cerr << "stdm-erase:"<<timevaldiff( &s, &f) << " sz:" << mm.size() << std::endl;    
}
