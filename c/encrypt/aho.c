#include <stdio.h>



/*
   �Ź沽���֥롼����
   ���ᤷ
   
 */


/*
  ʸ����Ϥ���֤���64bit���åȤǤ��Ĥ�����
  
 */
typedef struct
{
    char buf[8];
} string8;

void initSwap( string8 *k );
void initSwap_( string8 *k );
int rkHash( unsigned long *dm , char *pattern , int patlen );

int main()
{

    string8 a;
    a.buf[0] = 'a';
    a.buf[1] = 'b';
    a.buf[2] = 'c';
    a.buf[3] = 'd';
    a.buf[4] = 'e';
    a.buf[5] = 'f';
    a.buf[6] = 'g';
    a.buf[7] = 'h';
        
    hash64b( &a );

    return 0;
}

/*
 *
 */
string8 E( string8 k , string8 data )
{
    initSwap( &k );
    
}

/*
 * ���ž�֤򤹤롣���εա�
 * �ǽ�ϲ���ʤ��Ǥ褤��
 * ����Ф�Ȥ��ϡ������ޤȤ�ʤ�Τˤ��롣
 */
void initSwap( string8 *k )
{
    ;
}
void initSwap_( string8 *k )
{
    ;
}

/*
 * ʸ�����ѤΥϥå���ʤΤ� �Х��ʥ�ǡ����˻Ȥ��Ȥ���
 * ����äȥ��塼�˥󥰤�ɬ�פʤϤ���
 * �ޤ����롼�פ�ޤ魯���򳰤������Ƥ���褦�ˤ�����
 * ���ȤϤ��Τޤ޻Ȥ�������Ф�Ȥ��ˤ����Ȥ��롣
 * 32�ӥåȤ��ͤ��֤��Τǡ�64��ϥå��夹��ˤ�
 * Ⱦʬ�ˤ櫓�롣
 */
#define PRIME 211
int hashpjw ( char* s , int len )
{
    char *p;
    unsigned int h= 0 ,g;
    int i;
    p = s;
    for( i = 0 ; i < len ; i++ ){
        h = ( h<< 4 ) + (*p++);
        if( (g = h & 0xf0000000) != 0){
            h = h ^ (g>>24);
            h = h ^ g;
        }
        printf( "H:%d\n" ,h );
    }
    return h % PRIME;
}
int hash64b( string8 a )
{
    char b1[4] , b2[4];
    int i1,i2;
    
    memcpy( b1 , a.buf , 4 );
    memcpy( b2 , a.buf+4 , 4 );

/*    i1 = hashpjw( b1 , 4 );
    i2 = hashpjw( b2 , 4 );  */

    {
        unsigned long i = 239393;
        i1 = rkHash( &i , "ahoaho" , 6  );
        i2 = rkHash( &i , "ahoahp" , 6  );
        printf( "%d %d\n" , i1,i2 );
    }
    
}

#define Q 33554393L
#define LOG2D 8

int rkHash( unsigned long *dm , char *pattern , int patlen )
{
    int i;
    unsigned long h1;
    for(i=1,*dm = 1;  i < patlen ; i++ ){
        *dm = (*dm << LOG2D ) % Q;
    }
    for(i=0,h1=0;i<patlen;i++){
        h1 = (( h1 << LOG2D ) + pattern[i] ) % Q;
    }
    return h1;
    
}

