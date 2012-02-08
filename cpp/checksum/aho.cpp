#include <stdlib.h>
#include <string.h>
#include <stdio.h>

class Checksum {
public:
    Checksum() { clear(); }
    void clear() { sum = 0; r = 55665; c1 = 52845; c2 = 22719;}
    void add(const char *s, size_t l);
    void add( char b);
    unsigned int get() { return sum; }
protected:
    int r;
    int c1;
    int c2;
    unsigned int sum;
};



void Checksum::add( char value){
    unsigned char cipher = (value ^ (r >> 8));
    r = (cipher + r) * c1 + c2;
    sum += cipher;
}

void Checksum::add( const char *s, size_t l ){
    for(int i = 0; i < l; i++){
        add(s[i]);
    }
} 


#define CRCPOLY2 0xEDB88320UL  /* 左右逆転 */
#define CHAR_BIT 8
static unsigned long crc2(int n, unsigned char c[])
{
	int i, j;
	unsigned long r;

	r = 0xFFFFFFFFUL;
	for (i = 0; i < n; i++) {
		r ^= c[i];
		for (j = 0; j < CHAR_BIT; j++)
			if (r & 1) r = (r >> 1) ^ CRCPOLY2;
			else       r >>= 1;
	}
	return r ^ 0xFFFFFFFFUL;
}


int main() {
    char aho[] =  "akdhflkajsdhflkajhsdlkfjahsldkjfhaksjdfasd";
    char hoge[] = "akdhflkajsdhflkajhsdlkfjakslakjfhaksjdfkas";


    Checksum ck1,ck2;
    ck1.add(aho,strlen(aho));
    ck2.add(hoge,strlen(hoge));

    printf("%u\n", ck1.get() );
    printf("%u\n", ck2.get() );

    printf("crc1:%u\n", crc2( strlen(aho), (unsigned char*)aho ) );
    printf("crc2:%u\n", crc2( strlen(hoge), (unsigned char*)hoge ) );    
}
