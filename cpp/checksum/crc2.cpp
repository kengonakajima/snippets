#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_BIT 8

// 安価でけっこういいチェックサム
#define CRCPOLY2 0xEDB88320UL  /* 左右逆転 */

unsigned long crc2(const unsigned char *c, int n)
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

#define elementof(x) ( ( sizeof(x) / sizeof(x[0])))

int main() {
    char *aho[] = {"hoge", "akdjfasdjf;lakjsdf;lkasjdf", "あほあほあほ", "aa", "a", "aaa", "b", "bb" };
    for(int i=0;i<elementof(aho);i++){
        unsigned long c = crc2( (const unsigned char*)aho[i], strlen(aho[i]));
        printf("%u\n", c );
    }    
}
