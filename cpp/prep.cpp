#include <stdio.h>

class Y
{
    char a[100];
};

class X
{
    int a;
    Y y;
};

class Z
{
    int a,b,c,d,e;
};


int aho[ false ? 1 : 2 ];


#define max(a,b) ( (a) > (b) ? (a)  : (b) )


int poo[ max( sizeof(X), max( sizeof(Y), max( sizeof(Z), 1 ) ) ) ];
//int qqq[ max(4,max(4,max(4,max(4,max(4,max(4,max(4,1)))))))) ];
//int qqq[ max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,1)))))))))))))) ] ;
//int qqq[ max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,1)))))))))))))))))))))))];

#define max2(a,b) ( (a) > (b) ? (a)  : (b) )


//int qqq[ max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max(4,max2(4,max2(4,max2(4,max2(4,max2(4,max2(4,max2(4,max2(4,max2(4,max2(4,1)))))))))))))))))))))))];


class P {
    int a[100];
 public:
    P(int i){a[0]=i;}
    P(){}
};

 union aaaaaaaaaaa {
    int a;
    P p;
 };

int main()
    
{
    printf("%u\n", sizeof(aaaaaaaaaaa));
}
