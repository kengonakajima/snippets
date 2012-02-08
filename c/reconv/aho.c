#include <stdio.h>

#define TBLSIZE 256
char simple_enc_table[TBLSIZE] = {
    18,26,239,100,  231,164,65,93,
    147,118,59,176,  107,107,60,89,
    95,40,232,145,  86,171,114,70,
    137,131,72,109,  21,150,206,168,

    238,28,60,3,  216,178,13,76,
    195,233,219,104,  239,203,81,213,
    119,179,6,116,  146,239,28,83,
    9,220,193,142,  77,53,27,214,

    231,52,180,114,  99,60,17,247,
    227,125,151,237,  130,132,66,15,
    150,195,63,45,  10,81,212,222,
    212,2,155,157,  237,173,253,57,

    108,162,68,46,  251,47,74,106,
    125,87,42,43,  5,166,246,218,
    47,117,118,186,  199,103,10,178,
    104,104,20,128,  252,251,242,63
};
    
/* 可逆の，「不意に見る」を防ぐだけの暗号化.
   「アルゴリズム辞典 2ページ「暗号」
 */
int simple_2way_encrypt( char *out, int outlen, char *s, int len )
{
    int i;
    int total_c = 0;
    char hoge[16]={
        '0','1','2','3',  '4','5','6','7',
        '8','9','a','b',  'c','d','e','f' };

    if(outlen < (len*2+1)) return -1;
    
    for(i=0;i<len;i++){
        unsigned char c = s[i];
        unsigned char c1,c2;

        c = c ^ simple_enc_table[total_c%TBLSIZE];
        
        c1 = (c & 0xf0) >> 4;
        c2 = (c & 0x0f);
        
        out[i*2]= hoge[c1];
        out[i*2+1]= hoge[c2];
        total_c += s[i];
    }
    out[i*2]='\0';
    return 0;
}
int simple_2way_decrypt( char *out, int outlen, char *s, int len )
{
    int i;
    int total_c=0;
    if( outlen < (len/2+1)) return -1;
    for(i=0;i<len/2;i++){
        unsigned char c1[2];
        unsigned char c2[2];        

        c1[0] = s[i*2];
        c1[1] = '\0';

        c2[0] = s[i*2+1];
        c2[1] = '\0';
        
        out[i] = (strtoul(c1,NULL,16)<<4)+strtoul(c2,NULL,16);
        out[i] = out[i] ^ simple_enc_table[total_c%TBLSIZE];
        total_c += out[i];
    }
    out[i]='\0';
    return 0;
}



int main(int argc, char **argv )
{

    char hoge[100], moge[100];

    sprintf( hoge, "%s", argv[1]);

    simple_2way_encrypt( hoge,sizeof(hoge), argv[1], strlen(argv[1]));

    printf( "1: '%s'\n", hoge);

    simple_2way_decrypt( moge,sizeof(moge), hoge, strlen(hoge));

    printf( "2: '%s'\n",moge);

    return 0;
}
