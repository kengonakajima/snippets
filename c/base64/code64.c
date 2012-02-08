#define _CODE64_C_



/*

 ���̤�base64�ȤۤȤ��Ʊ����


  �ǡ������̤�33%�դ���Τ���


 */

/* �ץ�ȥ����פϤ������饳�ԡ����� */
void encode64( unsigned char *in , int i, unsigned char *out );
int decode64( unsigned char *in , unsigned char *out );

/*
 *  base64�ߤ����ʤ��Ȥ򤹤�ؿ���
 *  ���ʤ餺1�Х����Ѵ������1ʸ���ʾ��ʸ�����Ǥ��뤳�Ȥ����Ѥ��ơ�
 *  ���󤸤��ʥ��르�ꥺ��ˤ���Τ���
 *
 * char *in : ���ϥǡ���(�Х��ʥ�)
 * int len : ���ϥǡ�����Ĺ�����Х��ʥ������ɬ�פ���
 * char *out : ����ʸ���󡣥̥�ʸ���Ǥ���äƤ��ġ�
 * Ĺ����33%���ä���Τ�
 *
 * �Хåե��Τʤ��������¤Ϥʤ���
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

/* �հ���ɽ���ʲ��Ϥ����Ĥ��륳���ɡ� */
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
    
    /* ���ʤ餺3ʸ����1�� */

    for(i=0;;i+=3){
        unsigned char in1 , in2 , in3;
        unsigned char out1 ,out2 , out3 , out4;
        
        if( i >= len ) break;
        if( i >= (len-1)){
            /* �Ǹ��1ʸ��������Ƭ�ΤȤ� */
            in1 = in[i] & 0xff;
            in2 = in3 = 0;
            use_bytes = 2;
        } else if( i >= (len-2)){
            /* �Ǹ��2ʸ��������Ƭ�ΤȤ� */
            in1 = in[i] & 0xff;
            in2 = in[i+1] & 0xff;
            in3 = 0;
            use_bytes = 3;
        } else {
            /* �Ǹ��3ʸ���������ΤȤ� */
            in1 = in[i] & 0xff;
            in2 = in[i+1] & 0xff;
            in3 = in[i+2] & 0xff;
            use_bytes = 4;
        }

        /* in1,in2,in3�˾��󤬤���ä��� */
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
 * �ǥ����ɤ��롣
 * char *in : ���󥳡��ɤ��줿64ʸ����
 * char *out : �Ǥ�ʸ����
 * return value
 * ���ϤΥХ��ȿ����������Ϥ�����ϤΤۤ������������ʤ�Τǡ����ϥХե��������ϤȤ��ʤ�
 * �����Ѱդ���н�ʬ�Ǥ��롣
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
            /* 1ʸ����������������������Ǥ�6�ӥåȤ��������̤��ʤ��ΤǼΤƤ� */
            break;
        } else if( in[i+2] == 0 ){
            /* 2ʸ������������12bits */
            in1 = reversecharset[in[i]];
            in2 = reversecharset[in[i+1]];
            in3 = in4 = 0;
            use_bytes = 1;
        } else if( in[i+3] == 0 ){
            /* 3ʸ���������� 18 bits*/
            in1 = reversecharset[in[i]];
            in2 = reversecharset[in[i+1]];
            in3 = reversecharset[in[i+2]];
            in4 = 0;
            use_bytes = 2;
        } else {
            /* 4ʸ������ 24 bits*/
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
            /* ���줬�ʤ��Ƚ����ʤ��Τ���ա� */
            break;
        }
    }

    return address;
}

