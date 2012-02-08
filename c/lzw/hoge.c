#include <stdio.h>

#define B00000000 0
#define B00000001 1
#define B00000010 2
#define B00000011 3
#define B00000100 4
#define B00000101 5
#define B00000110 6
#define B00000111 7
#define B00001000 8
#define B00001001 9
#define B00001010 10
#define B00001011 11
#define B00001100 12
#define B00001101 13
#define B00001110 14
#define B00001111 15
#define B00010000 16
#define B00010001 17
#define B00010010 18
#define B00010011 19
#define B00010100 20
#define B00010101 21
#define B00010110 22
#define B00010111 23
#define B00011000 24
#define B00011001 25
#define B00011010 26
#define B00011011 27
#define B00011100 28
#define B00011101 29
#define B00011110 30
#define B00011111 31
#define B00100000 32
#define B00100001 33
#define B00100010 34
#define B00100011 35
#define B00100100 36
#define B00100101 37
#define B00100110 38
#define B00100111 39
#define B00101000 40
#define B00101001 41
#define B00101010 42
#define B00101011 43
#define B00101100 44
#define B00101101 45
#define B00101110 46
#define B00101111 47
#define B00110000 48
#define B00110001 49
#define B00110010 50
#define B00110011 51
#define B00110100 52
#define B00110101 53
#define B00110110 54
#define B00110111 55
#define B00111000 56
#define B00111001 57
#define B00111010 58
#define B00111011 59
#define B00111100 60
#define B00111101 61
#define B00111110 62
#define B00111111 63
#define B01000000 64
#define B01000001 65
#define B01000010 66
#define B01000011 67
#define B01000100 68
#define B01000101 69
#define B01000110 70
#define B01000111 71
#define B01001000 72
#define B01001001 73
#define B01001010 74
#define B01001011 75
#define B01001100 76
#define B01001101 77
#define B01001110 78
#define B01001111 79
#define B01010000 80
#define B01010001 81
#define B01010010 82
#define B01010011 83
#define B01010100 84
#define B01010101 85
#define B01010110 86
#define B01010111 87
#define B01011000 88
#define B01011001 89
#define B01011010 90
#define B01011011 91
#define B01011100 92
#define B01011101 93
#define B01011110 94
#define B01011111 95
#define B01100000 96
#define B01100001 97
#define B01100010 98
#define B01100011 99
#define B01100100 100
#define B01100101 101
#define B01100110 102
#define B01100111 103
#define B01101000 104
#define B01101001 105
#define B01101010 106
#define B01101011 107
#define B01101100 108
#define B01101101 109
#define B01101110 110
#define B01101111 111
#define B01110000 112
#define B01110001 113
#define B01110010 114
#define B01110011 115
#define B01110100 116
#define B01110101 117
#define B01110110 118
#define B01110111 119
#define B01111000 120
#define B01111001 121
#define B01111010 122
#define B01111011 123
#define B01111100 124
#define B01111101 125
#define B01111110 126
#define B01111111 127
#define B10000000 128
#define B10000001 129
#define B10000010 130
#define B10000011 131
#define B10000100 132
#define B10000101 133
#define B10000110 134
#define B10000111 135
#define B10001000 136
#define B10001001 137
#define B10001010 138
#define B10001011 139
#define B10001100 140
#define B10001101 141
#define B10001110 142
#define B10001111 143
#define B10010000 144
#define B10010001 145
#define B10010010 146
#define B10010011 147
#define B10010100 148
#define B10010101 149
#define B10010110 150
#define B10010111 151
#define B10011000 152
#define B10011001 153
#define B10011010 154
#define B10011011 155
#define B10011100 156
#define B10011101 157
#define B10011110 158
#define B10011111 159
#define B10100000 160
#define B10100001 161
#define B10100010 162
#define B10100011 163
#define B10100100 164
#define B10100101 165
#define B10100110 166
#define B10100111 167
#define B10101000 168
#define B10101001 169
#define B10101010 170
#define B10101011 171
#define B10101100 172
#define B10101101 173
#define B10101110 174
#define B10101111 175
#define B10110000 176
#define B10110001 177
#define B10110010 178
#define B10110011 179
#define B10110100 180
#define B10110101 181
#define B10110110 182
#define B10110111 183
#define B10111000 184
#define B10111001 185
#define B10111010 186
#define B10111011 187
#define B10111100 188
#define B10111101 189
#define B10111110 190
#define B10111111 191
#define B11000000 192
#define B11000001 193
#define B11000010 194
#define B11000011 195
#define B11000100 196
#define B11000101 197
#define B11000110 198
#define B11000111 199
#define B11001000 200
#define B11001001 201
#define B11001010 202
#define B11001011 203
#define B11001100 204
#define B11001101 205
#define B11001110 206
#define B11001111 207
#define B11010000 208
#define B11010001 209
#define B11010010 210
#define B11010011 211
#define B11010100 212
#define B11010101 213
#define B11010110 214
#define B11010111 215
#define B11011000 216
#define B11011001 217
#define B11011010 218
#define B11011011 219
#define B11011100 220
#define B11011101 221
#define B11011110 222
#define B11011111 223
#define B11100000 224
#define B11100001 225
#define B11100010 226
#define B11100011 227
#define B11100100 228
#define B11100101 229
#define B11100110 230
#define B11100111 231
#define B11101000 232
#define B11101001 233
#define B11101010 234
#define B11101011 235
#define B11101100 236
#define B11101101 237
#define B11101110 238
#define B11101111 239
#define B11110000 240
#define B11110001 241
#define B11110010 242
#define B11110011 243
#define B11110100 244
#define B11110101 245
#define B11110110 246
#define B11110111 247
#define B11111000 248
#define B11111001 249
#define B11111010 250
#define B11111011 251
#define B11111100 252
#define B11111101 253
#define B11111110 254
#define B11111111 255

/* masks for first byte ( write )*/
int  modifymask_first[8][9]={
    {0, B00000001,B00000011,B00000111,B00001111,B00011111,B00111111,B01111111,B11111111},/* mod 0*/
    {0, B00000011,B00000111,B00001111,B00011111,B00111111,B01111111,B11111111,B11111111},/* mod 1*/
    {0, B00000111,B00001111,B00011111,B00111111,B01111111,B11111111,B11111111,B11111111},/* mod 2*/
    {0, B00001111,B00011111,B00111111,B01111111,B11111111,B11111111,B11111111,B11111111},/* mod 3*/
    {0, B00011111,B00111111,B01111111,B11111111,B11111111,B11111111,B11111111,B11111111},/* mod 4*/
    {0, B00111111,B01111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111},/* mod 5*/
    {0, B01111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111},/* mod 6*/
    {0, B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111},/* mod 7*/
};
/* masks for second byte ( write ) */
int modifymask_second[8][9]={
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000},/* mod 0 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001},/* mod 1 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000011},/* mod 2 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000011,B00000111},/* mod 3 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000001,B00000011,B00000111,B00001111},/* mod 4 */
    {0, B00000000,B00000000,B00000000,B00000001,B00000011,B00000111,B00001111,B00011111},/* mod 5 */
    {0, B00000000,B00000000,B00000001,B00000011,B00000111,B00001111,B00011111,B00111111},/* mod 6 */
    {0, B00000000,B00000001,B00000011,B00000111,B00001111,B00011111,B00111111,B01111111},/* mod 7 */
};

/* masks for first byte ( read ) */
int readmask_first[8][9]={
    {0, B00000001,B00000011,B00000111,B00001111,B00011111,B00111111,B01111111,B11111111},/*mod0*/
    {0, B00000010,B00000110,B00001110,B00011110,B00111110,B01111110,B11111110,B11111110},/*mod1*/
    {0, B00000100,B00001100,B00011100,B00111100,B01111100,B11111100,B11111100,B11111100},/*mod2*/
    {0, B00001000,B00011000,B00111000,B01111000,B11111000,B11111000,B11111000,B11111000},/*mod3*/
    {0, B00010000,B00110000,B01110000,B11110000,B11110000,B11110000,B11110000,B11110000},/*mod4*/
    {0, B00100000,B01100000,B11100000,B11100000,B11100000,B11100000,B11100000,B11100000},/*mod5*/
    {0, B01000000,B11000000,B11000000,B11000000,B11000000,B11000000,B11000000,B11000000},/*mod6*/
    {0, B10000000,B10000000,B10000000,B10000000,B10000000,B10000000,B10000000,B10000000},/*mod7*/    
};
/* masks for second byte ( read ) */
int readmask_second[8][9]={
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000},/*mod0*/
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001},/*mod1*/
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000011},/* mod 2 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000000,B00000001,B00000011,B00000111},/* mod 3 */
    {0, B00000000,B00000000,B00000000,B00000000,B00000001,B00000011,B00000111,B00001111},/* mod 4 */
    {0, B00000000,B00000000,B00000000,B00000001,B00000011,B00000111,B00001111,B00011111},/* mod 5 */
    {0, B00000000,B00000000,B00000001,B00000011,B00000111,B00001111,B00011111,B00111111},/* mod 6 */
    {0, B00000000,B00000001,B00000011,B00000111,B00001111,B00011111,B00111111,B01111111},/* mod 7 */
};

/*
 * used by bitstream routines
 */
int bitstream_maxbyte, bitstream_bitaddr ;
char *bitstream_buf;

/* initialize bitstream for output */
int initOutputBitStream( char *buf ,int buflen)
{
    bitstream_bitaddr = 0;
    bitstream_maxbyte = buflen;
    bitstream_buf = buf;
    bzero( buf,buflen);
    return 0;
}
/* initialize bitstream for input */
int initInputBitStream( char *buf , int buflen)
{
    bitstream_bitaddr = 0;
    bitstream_maxbyte = buflen;
    bitstream_buf = buf;
    return 0;
}

/*
 * read from bit stream. used only from 1 bit to 8 bits
 * this is a base routine
 */
static unsigned int readInputBitStreamBody( int bwidth )
{
    int mod = bitstream_bitaddr % 8;
    int byteaddr = bitstream_bitaddr / 8;
    int i;
    int ret;
    
    /* return if excess */
    if( byteaddr >= bitstream_maxbyte)return 0;
    if( bwidth >= 1 && bwidth <= 8){
        int b1 = (( bitstream_buf[byteaddr] & modifymask_first[mod][bwidth] )>>mod);
        int b2 = (( bitstream_buf[byteaddr+1] & modifymask_second[mod][bwidth])<<(8-mod));
        bitstream_bitaddr += bwidth;
        return  b1 | b2;
    } else {
        return 0;
    }
}
/*
 *  read from bit stream. used from 1 bit to 32 bits
 *
 */
unsigned int readInputBitStream( int bwidth )
{
    if( bwidth <= 0 ){
        return 0;
    } else if( bwidth >= 1 && bwidth <= 8 ){
        return readInputBitStreamBody( bwidth );
    } else if( bwidth >= 9 && bwidth <= 16 ){
        unsigned int first = readInputBitStreamBody(8);
        unsigned int second = readInputBitStreamBody( bwidth-8);
        return first + (second << 8 );
    } else if( bwidth >= 17 && bwidth <= 24 ){
        unsigned int first = readInputBitStreamBody(8);
        unsigned int second = readInputBitStreamBody(8);
        unsigned int third = readInputBitStreamBody(bwidth-8);
        return first + ( second << 8 ) + ( third << 16 );
    } else if( bwidth >= 25 && bwidth <= 32 ){
        unsigned int first = readInputBitStreamBody(8);
        unsigned int second = readInputBitStreamBody(8);
        unsigned int third = readInputBitStreamBody(8);
        unsigned int forth = readInputBitStreamBody(bwidth-8);
        return first + ( second << 8 ) + ( third << 16 ) + ( forth << 24 );
    }
}
    
/*
 * write to a bitstream. only used from 1 bit to 8 bits
 * this is a base routine.
 */
static int writeOutputBitStreamBody( int bwidth , unsigned char b)
{
    int mod = bitstream_bitaddr % 8;
    int byteaddr = bitstream_bitaddr / 8;
    
    /* return error if excess */
    if( bitstream_maxbyte <= (byteaddr+1)) return -1;

    bitstream_buf[byteaddr] &= modifymask_first[mod][bwidth];
    bitstream_buf[byteaddr] |= (b << mod) & modifymask_first[mod][bwidth];
    bitstream_buf[byteaddr+1] &= modifymask_second[mod][bwidth];
    bitstream_buf[byteaddr+1] |= (b>>(8-mod))& modifymask_second[mod][bwidth];
    bitstream_bitaddr += bwidth;
    return byteaddr+1;
}

/*
 * write to a bitstream. used from 1 bits to 32 bits
 * returns -1 if error or buffer excession
 */
int writeOutputBitStream( int bwidth, unsigned int dat)
{
    int ret;
    if( bwidth <= 0){
        return -1;
    } else if( bwidth >= 1 && bwidth <= 8 ){
        if((ret=writeOutputBitStreamBody( bwidth , (unsigned char)dat))<0)return -1;
    } else if( bwidth > 8 && bwidth <= 16 ){
        if(writeOutputBitStreamBody( 8 , (unsigned char)(dat&0xff))<0)return -1;
        if((ret=writeOutputBitStreamBody( bwidth - 8 , ( unsigned char)((dat>>8)&0xff)))<0)return -1;
    } else if( bwidth > 16 && bwidth <= 24 ){
        if(writeOutputBitStreamBody( 8 , (unsigned char)(dat&0xff))<0)return -1;
        if(writeOutputBitStreamBody( 8 , (unsigned char)((dat>>8)&0xff))<0)return -1;
        if((ret=writeOutputBitStreamBody( bwidth-16,(unsigned char)((dat>>16)&0xff)))<0)return -1;
    } else if( bwidth > 24 && bwidth <= 32 ){
        if(writeOutputBitStreamBody( 8 , (unsigned char)(dat&0xff))<0)return -1;
        if(writeOutputBitStreamBody( 8 , (unsigned char)((dat>>8)&0xff))<0)return -1;
        if(writeOutputBitStreamBody( 8 , (unsigned char)((dat>>16)&0xff))<0)return -1;
        if((ret=writeOutputBitStreamBody( bwidth-24,(unsigned char)((dat>>24)&0xff)))<0)return -1;
    } else {
        return -1;
    }
    return ret;
}

#define CHAR_SIZE 256
#define NODE_SIZE 512
#define BITS_LEN 9          /* 9 bit lzw compression */

typedef struct {
    unsigned char chr;
    int parent;
    int brother;
    int child;
}NODE;

long ringoCompressor( unsigned char *code , long codelen ,
            unsigned char *text , long textlen)
{
    NODE node[NODE_SIZE];
    int freeNode;        

    int w,k;        /* used in this algo */
    int textind;    /* index to text buffer */
    int i;
    int position;   /* indicates the last byte of code buffer */
    
    if( textlen <= 0 ) return -1;

    initOutputBitStream( code,codelen);
    
    /* fill characters ( 0 ~ 255 ) in the beggining part of
       Node list */
    for(i=0; i<= CHAR_SIZE; i++){
        node[i].chr = (unsigned char)i;
        node[i].brother = i + 1;
        node[i].parent = 0;
        node[i].child = 0;
    }
    node[CHAR_SIZE].brother = 0;
    freeNode = CHAR_SIZE + 1;

    w = text[0];
    textind = 1;
    while(1){
        int rv;
        
        if( textind >= textlen ){
            k = CHAR_SIZE;      /* indicates EOF */
        } else {
            k = text[textind];
        }

        /* search if pattern 'wk' is registered or not. */
        rv = node[w].child;
        while(1){
            if( rv <= 0 ) break;
            if( node[rv].chr == k ) break;
            rv = node[rv].brother;
        }
        if( rv > 0 ){
            /* found it */
            w = rv;
        } else {
            position = writeOutputBitStream( BITS_LEN ,w  );

            /* return if buffer excession  */
            if( position > codelen ) return -1;
                
            /* register pattern 'wk' in the dictionary */
            if( freeNode < NODE_SIZE ){
                node[freeNode].parent = w;
                node[freeNode].chr = k;
                node[freeNode].brother = node[w].child;
                node[freeNode].child = 0;
                node[w].child = freeNode;
                freeNode++;
            }
            w = k;
        }
        if( textind == ( textlen + 1 )  ) break;
        textind++;
    }
    return position;
}

/*
 * Decoder.
 * return -1 if buffer excession. Notice buffer text
 * is modified .
 */
long ringoDecompressor( unsigned char *text , long textlen ,
            unsigned char *code , long codelen)
{
    NODE node[NODE_SIZE];
    int stack[NODE_SIZE];
    int sp;
    int freeNode;
    int len;
    int i;
    int k = 0;
    int w;
    
    if( codelen <= 0 ) return -1;
    initInputBitStream( code , codelen );

    for(i=0;i<CHAR_SIZE;i++){
        node[i].chr = (unsigned char)i;
        node[i].brother = i+1;
        node[i].parent = 0;
        node[i].child = 0;
    }

    node[CHAR_SIZE].brother = 0;
    freeNode = CHAR_SIZE + 1;
    
    len=0;  /* index to text buffer */
    sp=0;   /* stack pointer */
    
    while(1){
        int rv = 0;
        rv = readInputBitStream( BITS_LEN );

        /* return OK if EOF */
        if( rv == CHAR_SIZE ) break;        
        if( rv >= freeNode ){
            stack[sp++] = k;            /* exception */
            if( sp >=( sizeof( stack ) /sizeof(stack[0] )) )return -1;   
            k = w;
        } else {
            k = rv;
        }

        while(k> CHAR_SIZE ){
            if( k >= (sizeof(node)/sizeof(node[0]))) return -1;
            stack[sp++] = node[k].chr;
            k = node[k].parent;
            if( sp >=( sizeof( stack ) /sizeof(stack[0] )) ) return -1;
        }
        stack[sp++] = k;
        if( sp >= ( sizeof( stack ) /sizeof(stack[0] )) ) return -1;

        /* output to text buffer from stack.*/
        while(sp){
            if( ++len > textlen ) return -1;
            *text++ = stack[--sp];
        }
        /* register the pattern 'wk'*/
        if( len > 1 && freeNode < NODE_SIZE ){
            node[freeNode].parent = w;
            node[freeNode].chr = k;
            if( w >= (sizeof(node)/sizeof(node[0])))return -1;
            node[freeNode].brother = node[w].child;
            node[freeNode].child = 0;
            node[w].child = freeNode;
            freeNode++;
        }
        w = rv;
    }
    return len;
}

