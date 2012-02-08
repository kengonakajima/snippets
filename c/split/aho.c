#include <stdio.h>


char data[] = " \t 100  200     \t     400";

void easyGetTokenFromString( char *src , int count , char*output , int len );
void strcpysafe( char* dest ,size_t n ,const char* src );
char* strncpy2( char* dest, const char* src, size_t n );

main()
{
    char out[100];
    int a,b,c;
    

    easyGetTokenFromString( data ,1, out , sizeof(out) );
    a = atoi( out );
    printf("hoge:'%s'\n" , out );
    easyGetTokenFromString( data , 2, out , sizeof(out) );
    b = atoi(out);
    printf("hoge:'%s'\n" , out );
    easyGetTokenFromString( data , 3, out , sizeof(out) );
    c=atoi( out);
    printf("hoge:'%s'\n" , out );
    printf( "%d %d %d\n" , a ,b,c );
}

/*
 * ����ե������ɤ��ѤΡ�split.
 * ���������פˤ��б����Ƥ��ʤ���
 * ���֤Ȥ����ڡ����������Ĥ��äƤ⤽�줬�ǥ�ߥ��Ȥʤ롣
 * ����ե�����Ϥ褯�����줤�ˤʤ�٤뤿��˥��ڡ����Ȥ����֤�����ޤ��뤬��
 * ����Ǥ�������ɤߤ����褦�ˤ��뤿��Τ�Ρ�
 * char *src : ����ʸ����
 * int count : �ǽ�Υȡ�����ʤ�1 ����ʹ�2��3��4..
 * char *output : ����
 * int len : output�˥��ԡ���ǽ��Ĺ��
 */
void easyGetTokenFromString( char *src , int count , char*output , int len )
{
    int i;
    int counter = 0;
    
    if( len <= 0 )return;

    
#define ISSPACETAB( c )   ( (c) == ' ' || (c) == '\t' )
    
    for(i=0;;i++){
        if( src[i]=='\0'){
            output[0] = '\0';
            return;
        }
        if( i > 0 && ! ISSPACETAB( src[i-1] ) &&
            ! ISSPACETAB(  src[i] ) ){
            continue;
        }
        
        if( ! ISSPACETAB( src[i]) ){
            counter++;
            if( counter == count){
                /* copy it */
                int j;
                for(j=0;j<len-1;j++){
                    if( src[i+j] == '\0' ||
                        ISSPACETAB( src[i+j] ) ){
                        break;
                    }
                    output[j]=src[i+j];
                }
                output[j]='\0';
                return;
                
            }
        }

    }
    
}


void strcpysafe( char* dest ,size_t n ,const char* src )
{
    /*
     * src ���� dest �إ��ԡ�����.
     * strcpy, strncpy �Ǥ� dest ��� ���ԡ������̤�
     * �礭�����˸Ƥ֤�,���곰��������������.
     * ������ɤ��٤�, strncpy �����뤬 strlen( src ) �� n ���
     * �礭�����ˤ�, dest �κǸ夬 NULL ʸ���ȤϤʤ�ʤ�.
     *
     * �������ä� dest ���礭����� src �Τۤ���Ĺ�����ˤ�
     * n-1 �� strncpy �򤹤�. ���������Ϥ��Τޤޥ��ԡ�����
     *
     * n ����λ��Ϥ��������ʤ�Τ���λ��� ���⤷�ʤ���
     *
     */

    if( n <= 0 )        /* ���⤷�ʤ�   */
        return;

    /*  ���λ����ǡ� n >= 1 �ʾ夬����  */
    /*  NULLʸ�����θ������Ӥ���  */
    else if( n < strlen( src ) + 1 ){
        /*
         * �Хåե���­��ʤ��Τ� n - 1(NULLʸ��)
         * �� strncpy ��Ƥ�
         */
        strncpy2( dest , src , n-1 );
        dest[n-1]='\0';
    }else
        strcpy( dest , src );

}


/*------------------------------------------------------------
 * EUC��2�Х����ܤ����򥳥ԡ����ʤ�strncpy
 * ����
 *  dest        char*   ���ԡ���
 *  src         char*   ������
 *  n           size_t  Ĺ��
 * �֤���
 *  dest
 ------------------------------------------------------------*/
char* strncpy2( char* dest, const char* src, size_t n )
{
    if( n > 0 ){
        char*   d = dest;
        const char*   s = src;
        int i;
        for( i=0; i<n ; i++ ){
            if( *(s+i) == 0 ){
                /*  ���ԡ�������ä��� NULL ʸ���������   */
                *(d+i) = '\0';
                return dest;
            }
            if( *(s+i) & 0x80 ){
                *(d+i)  = *(s+i);
                i++;
                if( i>=n ){
                    *(d+i-1)='\0';
                    break;
                }
                *(d+i)  = *(s+i);
            }else
                *(d+i) = *(s+i);
        }
    }
    return dest;
}

