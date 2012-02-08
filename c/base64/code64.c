#define _CODE64_C_



/*

 普通のbase64とほとんど同じ。


  データの量が33%ふえるのだ。


 */

/* プロトタイプはここからコピーしろ。 */
void encode64( unsigned char *in , int i, unsigned char *out );
int decode64( unsigned char *in , unsigned char *out );

/*
 *  base64みたいなことをする関数。
 *  かならず1バイト変換すると1文字以上の文字ができることを利用して、
 *  たんじゅんなアルゴリズムにするのだ。
 *
 * char *in : 入力データ(バイナリ)
 * int len : 入力データの長さ。バイナリだから必要だ。
 * char *out : 出力文字列。ヌル文字でおわってるやつ。
 * 長さは33%増加するのだ
 *
 * バッファのながさに制限はない。
 */
char charset[64]={
    'A','B','C','D',    'E','F','G','H',
    'I','J','K','L',    'M','N','O','P',
    'Q','R','S','T',    'U','V','W','X',
    'Y','Z','a','b',    'c','d','e','f',
    'g','h','i','j',    'k','l','m','n',
    'o','p','q','r',    's','t','u','v',
    'w','x','y','z',    '0','1','2','3',
    '4','5','6','7',    '8','9','+','-'
};

/* 逆引き表。以下はそれをつくるコード。 */
/*
    int i;
    char re[128];
    for(i=0;i<64;i++)re[ charset[i]] = i;
    for(i=0;i<128;i++)printf( "%d," , re[i] );
 */
char reversecharset[256]={
    0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0,
    0,0,0,62, 0,63,0,0,
    52,53,54,55, 56,57,58,59,
    60,61,0,0, 0,0,0,0,
    0,0,1,2, 3,4,5,6,
    7,8,9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22,
    23,24,25,0, 0,0,0,0,
    0,26,27,28, 29,30,31,32,
    33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48,
    49,50,51,0, 0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0,
    0,0,0,0,  0,0,0,0
};

/*
 *  00000000 00000000 00000000
 *  000000/ 00 0000/ 0000 00/ 000000
 */
void encode64( unsigned char *in , int len , unsigned char *out )
{
    int i;
    int use_bytes;
    int address = 0;

    out[0] = 0;
    
    /* かならず3文字で1周 */

    for(i=0;;i+=3){
        unsigned char in1 , in2 , in3;
        unsigned char out1 ,out2 , out3 , out4;
        
        if( i >= len ) break;
        if( i >= (len-1)){
            /* 最後の1文字前が先頭のとき */
            in1 = in[i] & 0xff;
            in2 = in3 = 0;
            use_bytes = 2;
        } else if( i >= (len-2)){
            /* 最後の2文字前が先頭のとき */
            in1 = in[i] & 0xff;
            in2 = in[i+1] & 0xff;
            in3 = 0;
            use_bytes = 3;
        } else {
            /* 最後の3文字前以前のとき */
            in1 = in[i] & 0xff;
            in2 = in[i+1] & 0xff;
            in3 = in[i+2] & 0xff;
            use_bytes = 4;
        }

        /* in1,in2,in3に情報がそろった。 */
        out1 = ((in1 & 0xfc)>>2) & 0x3f;
        out2 = ((in1 & 0x03)<<4) | ((( in2 & 0xf0)>>4)&0x0f);
        out3 = ((in2 & 0x0f)<<2) | ((( in3 & 0xc0)>>6)&0x03);
        out4 = (in3 & 0x3f );

        if( use_bytes >= 2 ){
            out[address++] = charset[out1];
            out[address++] = charset[out2];
            out[address]=0;
        }
        if( use_bytes >= 3 ){
            out[address++] = charset[out3];
            out[address]=0;
        }
        if( use_bytes >= 4 ){
            out[address++] = charset[out4];
            out[address]=0;
        }
        
        printf( "IN: %x %x %x OUT: %x %x %x %x\n" , in1 ,in2,in3, out1,out2,out3,out4 ); 
    }
    
}

/*
 * デコードする。
 * char *in : エンコードされた64文字列
 * char *out : 素の文字列
 * return value
 * 出力のバイト数。あと入力よりも出力のほうがちいさくなるので、出力バファーは入力とおなじ
 * だけ用意すれば十分である。
 */
int decode64( unsigned char *in , unsigned char *out )
{
    unsigned char in1 , in2 , in3 , in4;
    unsigned char out1 , out2 , out3;
    int use_bytes;
    int address= 0;
    int i;

    for(i=0;;i+=4 ){
        if( in[i] == 0 ){
            break;
        } else if( in[i+1] == 0 ){
            /* 1文字だけ処理。しかしそれでは6ビットしか情報量がないので捨てる */
            break;
        } else if( in[i+2] == 0 ){
            /* 2文字だけ処理。12bits */
            in1 = reversecharset[in[i]];
            in2 = reversecharset[in[i+1]];
            in3 = in4 = 0;
            use_bytes = 1;
        } else if( in[i+3] == 0 ){
            /* 3文字だけ処理 18 bits*/
            in1 = reversecharset[in[i]];
            in2 = reversecharset[in[i+1]];
            in3 = reversecharset[in[i+2]];
            in4 = 0;
            use_bytes = 2;
        } else {
            /* 4文字処理 24 bits*/
            in1 = reversecharset[in[i]];
            in2 = reversecharset[in[i+1]];
            in3 = reversecharset[in[i+2]];
            in4 = reversecharset[in[i+3]];
            use_bytes = 3;
        }
        out1 =  (in1<<2) | (((in2 & 0x30)>>4)&0x0f) ;
        out2 =  ((in2 & 0x0f )<<4) | ((( in3 & 0x3c)>>2)&0x0f);
        out3 =  ( (in3 &0x03)<<6) |  ( in4 & 0x3f );

        printf( "IN:%x %x %x %x OUT: %x %x %x USE_BYTES:%d ADDRESS:%d\n" ,
                in1,in2,in3,in4,
                out1 , out2, out3 , use_bytes , address );
        
        if( use_bytes >= 1 ){
            out[address++] = out1;
        }
        if( use_bytes >= 2 ){
            out[address++] = out2;
        }
        if( use_bytes >= 3 ){
            out[address++] = out3;
        }
        if( use_bytes != 3 ){
            /* これがないと終わらないので注意。 */
            break;
        }
    }

    return address;
}

