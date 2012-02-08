#include <map>
#include <iostream>


std::map<int,int> mmm;

int main()
{
    int n=1000;
    int m=10000;
    for( int i=0;i<m;i++){
        mmm[i] = 0;
    }
    for( int i=0;i<n;i++){
        for( int j=0;j<m;j++){
            mmm[j] += 1;
        }
    }
    return 0;
}
