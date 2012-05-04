#include <stdio.h>
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
		for (j = 0; j < CHAR_BIT; j++) {
			if (r & 1){
                r = (r >> 1) ^ CRCPOLY2;
            } else {
                r >>= 1;
            }
        }
	}
	return r ^ 0xFFFFFFFFUL;
}

int main(int argc, char **argv ) {
    if(argc!=2){
        fprintf(stderr,"need 1 arg\n");
        return 1;
    }
    
    unsigned long out = crc2( argv[1], strlen( argv[1]));
    printf( "%u\n",(unsigned int)out );
    return 0;
}
