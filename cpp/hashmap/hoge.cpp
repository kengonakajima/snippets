#include <unordered_map>
#include <map>
#include <iostream>

#include <stdlib.h>

using namespace std;

int gen() {
    return random() & 0xffff;
}


// 10M, O3 : 3s sys 99%   init, searchが同じぐらいの時間かかる。
void testMap() {
    map<int,int> m;
    int n = 10000000;
    cerr << "init" << std::endl;
    for(int i=0;i<n;i++) {
        int r = gen();
        m[r] = i;
    }
    cerr << "search" << std::endl;
    long long tot=0;
    for(int i=0;i<n;i++) {
        int r = gen();
        tot += m[r];
    }
    cerr << "TOT:" << tot << endl;
}
// 10M, O3 : 0.6s sys 99% init,search だいたい同じ。
void testUnorderedMap() {
    unordered_map<int,int> u;
    int n = 10000000;
    cerr << "init" << std::endl;
    for(int i=0;i<n;i++) {
        int r = gen();
        u[r] = i;
    }
    cerr << "search" << std::endl;
    long long tot=0;
    for(int i=0;i<n;i++) {
        int r = gen();
        tot += u[r];
    }
    cerr << "TOT:" << tot << endl;    
}
// 10M, O3: 0.04s
void testEmpty() {
    long long tot=0;
    int n = 10000000;
    for(int i=0;i<n;i++) {
        int r = gen();
        tot += r;
    }
    cerr << "TOT:" << tot << endl;    
}

int main() {
    //    testMap();
        testUnorderedMap();
    // testEmpty();
}
