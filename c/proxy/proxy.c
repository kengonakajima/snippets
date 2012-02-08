


#include "common.h" /* ���̲����줿�ؿ��Υץ�ȥ����פȤ������� */

#include "proxy.h"


/*
 * �פ����򤹤����Ȥ���ʤ�
 * ���δؿ��Τʤ��ǤĤ����뤫�󤹤������� common.h �ˤϤ��äƤ�Ϥ�
 */

#define HTTP_DEFAULTPORT 9999        /* �Ǥդ���Ȥݡ��ȡ� */
#define FTP_DEFAULTPORT 9990


char linebuf[65536*2];            /* �إå�1�Ԥκ���ʤ��� */
char linebuf2[sizeof(linebuf)];            /* �إå�1�Ԥκ���ʤ��� */

int http_port = HTTP_DEFAULTPORT;
int ftp_port = FTP_DEFAULTPORT;

int mainsockfd;

#include <stdio.h>

int startProxy( int argc , char **argv )
{
#if 0
    /* �ʲ��Υ����ɤǥ����Ф�������ȤäƤ���ޤ��롣 */
    int s;
    s = p_connectToHTTPServer( "pingu" , 80 );
    while(1){
        char hoge[100];
        char aho[11111];
        
        /* ������������� */

        strcpy( hoge , "GET / HTTP/1.0\n\n");
        write( s , hoge , strlen( hoge ));

        sleep(1);
        {
            int r;
            r = read( s ,  aho, sizeof(aho));
            aho[r]=0;

            fprintf( stderr, "Data:[%s]\n", aho );
            fprintf( stderr, "Len:%d\n", strlen(aho) );
        }
    }
    
    return 0;
#endif
    
    /* �����С������åȤ����� */
    mainsockfd = p_initServerSocket( http_port , ftp_port );
    if( mainsockfd < 0 ) return 1;
    
    /* �������塼�롼�ץޥ路���㤤�ޤ��礦 */
    while(1){
        int i;
        
        /* �ݡ���󥰤���������ޤ��礦
           fd_set �Ȥ�����Τ�
         */
        p_preparePolling(mainsockfd);

        /* �ݡ���󥰤����㤤�ޤ��礦��
           �����ǥ����ॢ���Ȥλ���Ȥ��Ǥ���Τǡ�
           ������ prepare�� �櫓�� .
           ���ä����ˤ� doPolling����ȡ��⤦�ͥåȥ��
           ���� read ���Ƥ��ޤ��Ȥ��롣�����ƥХåե��ˤ���
           ����äƤ�Ȥ��롣
           */
        p_doPolling(mainsockfd);


        for(i=p_getMinPollingCount();
            i<p_getMaxPollingCount();i++){
            char buffer[5120];
            char req[1000]; /* �Ƥ��Ȥ������뤫������Ȥ���*/
            int r;


            
            if( p_isHTTP( i ) ){    /* HTTP�� */
                if( p_isHTTPHeaderState(i)){    /* �إå���ޤ��������Ƥʤ���*/
                    r = p_socketReadHTTPHeader(i,buffer,sizeof(buffer));
                    if( r <= 0 ) continue;

                    /* �إå�������֤����� */
                    if( p_isClientSocket(i)){   /* ���饤�������³. */

                        int s;

                        p_startHTTPBody(i); /* ���֤򤹤���� */
                        
                        getHTTPRequest( buffer , req ,sizeof(req));
                        p_print( req );
                        
                        s = p_connectToHTTPServer( "pingu" , 80 );
                        if(s>0){
                            p_socketWrite( s,"GET / HTTP/1.0\n\n" ,
                                           strlen( "GET / HTTP/1.0\n\n" ));
                            /* �դ��ĤΥ����åȤ��Ϣ�Ť��롣
                               i �ϥ��饤����ȤȤ���³�ǡ� s ��
                               �����ФȤ���³�� */
                            p_associateSocket( i , s );
                        }
                    } else {        /* �����С���³ */
                        p_startHTTPBody(i);
                        getHTTPRequest(buffer,req,sizeof(req));
                        p_print(req);
                    }
                } else {        /* �ؤä���������������� */
                    r = p_socketRead( i , buffer ,sizeof(buffer));

                    if( r <= 0 ) continue;
                    
                    if( p_isClientSocket(i)){   /* ���饤�������³��*/
                        p_socketWrite( p_getAssociatedSocket(i),
                                       buffer , r );
                    } else {    /* �����С���³�� */
                        int sa;
                        sa = p_getAssociatedSocket(i);
                        if( sa < 0 ){
                            /* ���饤����Ȥ������������ڤ�Ƥ��롣
                               ����ǤϻŻ��򤷤Ƥ��̣���ʤ��ʤ���
                               �Ȥ����㤨 */
                            fprintf( stderr, "Client closed first\n");
                        } else {
                            p_socketWrite( sa , buffer , r );
                        }
                    }
                }
            }
        }

        /* �ݡ���󥰽�λ�� */
        p_finishPolling( mainsockfd );
        
        
    }
    
    /* �����ΤǤȤ����㤤�ޤ��礦�� */
    p_closeServerSocket( mainsockfd );
    return 0;
}

/* �إå�����ꥯ�����ȹԤ�Ȥ������
   ��Ƭ������Ԥ�����ʸ���ޤǡ�
 */   
void getHTTPRequest( char *src , char *out , int maxlen )
{
    getStringFromIndexWithDelim( src , "\n" , 1, out , maxlen );        
}
/* �ꥯ�����ȹԤ����URL */
void getHTTPRequestURL( char *src , char *out , int maxlen )
{
    getHTTPRequest( src,linebuf, sizeof( linebuf));
    getStringFromIndexWithDelim( linebuf , " " , 2, out , maxlen );    
}
/* �ꥯ�����ȹԤ����Method */
void getHTTPRequestMethod( char *src , char *out , int maxlen )
{
    getHTTPRequest( src,linebuf, sizeof( linebuf));
    getStringFromIndexWithDelim( linebuf, " " , 1 , out , maxlen );        
}
/* http:// �Ȥ� ftp:// ��Ȥ������ */
void getHTTPRequestScheme(  char *src , char *out , int maxlen )
{
}
/* http://HOSTNAME/....  �� HOSTNAME��Ȥ���� */
void getHTTPRequestHostname( char *src , char *out , int maxlen )
{
}
/* http://HOSTNAME/FILENAME �� /FILENAME��Ȥ���� */
void getHTTPRequestFilename( char *src , char *out , int maxlen )
{
}


void getHTTPHost( char *src , char *out, int maxlen )
{
}


/*****************************************************************
  �Τ֤����Τ롼���󡣥������ƥ������¸�ʤ���
  
*****************************************************************/

#define min( x,y ) ({typeof(x) __x=(x),__y=(y);(__x < __y) ? __x : __y; })
#define max( x,y ) ({typeof(x) __x=(x),__y=(y);(__x < __y) ? __y : __x; })
#define swap( x,y )({typeof(x) __x=(y);(y)=(x);(x)=__x;})
#define FALSE 0
#define TRUE 0
#undef NULL 0
#define NULL 0


/* strstr �� linux �Υ����ͥ륳���ɤ����äƤ����ä��� */
char * strstr(const char * s1,const char * s2)
{
	int l1, l2;

	l2 = strlen(s2);
	if (!l2)
		return (char *) s1;
	l1 = strlen(s1);
	while (l1 >= l2) {
		l1--;
		if (!memcmp(s1,s2,l2))
			return (char *) s1;
		s1++;
	}
	return NULL;
}


char* strncpy2( char* dest, const char* src, int n )
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

/*----------------------------------------
 * src ���� dest �˥��ԡ����롣 ���ԡ���Υ�������Ĵ�٤롣
 * ����
 *      dest        ���ԡ���
 *      n           ������
 *      src         ����ʸ����
 ----------------------------------------*/
void strcpysafe( char* dest ,int n ,const char* src )
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

/*----------------------------------------
 * src ���� length �Ǥ���   ���ԡ�����礭����Ĵ�٤롣
 * ����
 *          dest        ���ԡ���
 *          n           ���ԡ���Υ�����
 *          src         ���ԡ���
 *          length      ��ʸ�����ԡ����뤫
 ----------------------------------------*/
void strncpysafe( char* dest , const int n ,
                  const char* src ,const int length )
{
    /*
     * src ���� dest �� length ���ԡ�����
     * strcpy, strncpy �Ǥ� dest ��� ���ԡ������̤�
     * �礭�����˸Ƥ֤�,���곰��������������.
     * ���δؿ��Ǥϡ�strlen( src ) �� length �ξ������ۤ�
     * (�ºݤ˥��ԡ�������) �� dest �Υ���������٤ơ�
     * strcpysafe ��Ʊ�����򤹤롣
     */

    int Short;
    Short = min( strlen( src ) , length );

    /* NULLʸ�� ���θ������� */
    if( n < Short + 1 ){
        /*
         * �Хåե���­��ʤ��Τ� n - 1(NULLʸ��)
         * �� strncpy ��Ƥ�
         */
        strncpy2( dest , src , n-1 );
        dest[n-1]='\0';

    }else if( n <= 0 ){
        return;
    }else{
        /*
         * �Хåե��Ͻ�ʬ�ˤ���Τ� Short ��strncpy��Ƥ�
         * �ʤ� src �ˤ� Short ��Ĺ����� NULL ���ʤ��Τǡ�
         * dest �ˤ� �ɲä��Ƥ�����
         */

        strncpy2( dest , src , Short );
        dest[Short]= '\0';

    }
}

/*------------------------------------------------------------
 * strcat������˼¸����롣�٤��Τǡ����̤ʾ�������ƻȤ�ʤ�����
 * ����
 *  src     char*       ���ˤʤ�ʸ����
 *  size    int         src �Υ�����( ���ˤɤ�����ĤäƤ��뤫�Ǥ�
 *                          �ʤ��ƥХåե����� )
 *  ap      char*       �դ��ä���ʪ
 * �֤���
 *  src     (�������)
 ------------------------------------------------------------*/
char* strcatsafe( char* src, int size, char* ap )
{
    strcpysafe( src + strlen( src ) , size - strlen(src), ap );
    return src;
}


int getStringFromIndexWithDelim( char* src ,char* delim ,int index,
                                 char* buf , int buflen)
{
    int i;          /* �롼���ѿ� */
    int length =0;  /* ���Ф���ʸ�����Ĺ�� */
    int addlen=0;   /* ­�����Ĺ�� */

    for( i =  0 ; i < index ; i ++ ){
        char* last;
        src += addlen;          /* �ߤĤ��ä�Ĺ����­�� */
        last  = strstr( src , delim );  /* ���դ��� */

        if( last == NULL ){
            /*
             * �ߤĤ���ʤ��ä��ΤǤ��٤ƥ��ԡ����� return��
             */
            strcpysafe( buf , buflen, src );
            if( i == index - 1 )
                /*���礦�ɤߤĤ��ä�*/
                return TRUE;

            /*�ߤĤ���ʤ��ä�*/
            return FALSE;
        }

        /*
         * �ߤĤ��ä���Ⱥǽ�ΰ��֤κ������
         * �Ĥޤ���ڤ��Ƥ���ʸ�����Ĺ��
         */
        length = last - src;

        /*
         * ���Υ롼�פΰ٤ˤߤĤ��ä�Ĺ���� delim ��Ĺ����­���Ƥ���
         */
        addlen= length + strlen( delim );
    }

    strncpysafe( buf, buflen , src,length );
    return TRUE;
}
