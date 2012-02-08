%{  

#include "proxy.h"  /* ���Υ����ɤ�����������ؿ��ϡ�proxy.h�ˤ����� */
    
    /* p �Ѥ� flex �����ɡ�
       �ʤ�Ǥ�������ޤä�����

     */

#undef YY_INPUT
#define YY_INPUT( b , r , ms ) ( r = yyinput(b,ms))

typedef enum
{
    R_HTTP_REQUEST,         /* HTTP �κǽ�� GET / HTTP/1.0 �ι� */
    R_HTTP_METHOD,          /* HTTP��GET/PUT/POST�ˤޤä����Ƥۤ����� */
} REGEXP;

REGEXP scantype;        /* �ʤˤ򥹥���󤷤ޤ��礦�� */
int scancount;          /* �����Ĥ�Ǥ��礦�� */
int current_scancount;   
%}    

HTTPMETHOD "GET"|"POST"|"PUT"
HTTPVERSION "HTTP/1."[01]

%%

[\t ]+         { ;}

^{HTTPMETHOD}       {  printf( "AAAAA\n");answer(R_HTTP_METHOD);  }

{HTTPMETHOD}" "(.+)" "{HTTPVERSION}\n  { answer(R_HTTP_REQUEST); }


. { printf( "CHAR:[%s]\n" , yytext ); }

%%

char *inputbuf;     /* �縵�ΥХåե� */
char *inputbuf_p;   /* �縵�ΥХåե��Τɤ����򤵤�pointer�� */
int inputbufmax;     /* ������ȤΥХåե��λ����� */
char *answerstring;       /* �����򤤤��ݥ��� */
int answerstring_max;     /* �������κ���Ĺ�� */


#define SETANS( out , len ) answerstring = (out); answerstring_max = (len);\
    answerstring[0] = '\0';   

#define SETSCAN( type , c ) scantype = (type); scancount = (c);\
    current_scancount = 0;
    
#define SETINPUT( src ) inputbuf=inputbuf_p=(src);inputbufmax=strlen(src);

#define LEX()       yylex();


/*
 * �إå����Τ��顢HTTP�Υꥯ�����ȹԤ�Ȥ������
 */
void getHTTPRequest( char *src , char *out , int maxlen )
{
    SETANS( out, maxlen );
    SETSCAN(R_HTTP_REQUEST , 1 );
    SETINPUT( src );
    LEX();

    
    
}

/*
 * yytext ����Ф��줿ʸ���󤫤顢�������������ʸ����
 * p_scan_answer �˥��ԡ����롣
 * ����ѥ�����˥ޥå������餳����֤�����
 *
 * ������Ӥ����줿�Ȥ����н��ͤ��ޤ��礦��
 *
 */
void answer( int rtype )
{
    if( scantype != rtype ) return;
    
    current_scancount++;
    if( current_scancount < scancount )return;
    
    if( answerstring){
        int i;
        for(i=0;i<answerstring_max-1;i++){
            answerstring[i] = yytext[i];
            if( yytext[i] == '\0' )break;
        }
        answerstring[i] = '\0';     /* ����餻�ʤ��Ȥ͡� */
    }
}

/*
 * ���Ф��ȥ��ԡ��������򤫤����Τ���
 *
 */
int yyinput( char *buf , int max_size )
{
    
    int n;
    
    /* �������줿�龡��˽̤�ΤǤ⤤���� */
    int last = inputbufmax - ( inputbuf_p - inputbuf );
    if( last < max_size ){
        memcpy( buf , inputbuf_p , last );
        inputbuf_p += last;
        return last;
    } else if( last == 0 ){
        return 0;
    } else {
        memcpy( buf , inputbuf_p , max_size );
        inputbuf_p += max_size;
        return max_size;
    }
}


