


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>



/*
     �פ�����  linux.c by ringo


     linux �Υ������ƥ�����˰�¸������ʬ�򤫤����Ȥ��äƤ⤽��ʤȤ���
     �Ϥʤ������̤�Unix�ʤ��Ȥ򤹤�������ޤä��Ȥ�������˰ܿ����褦��
     ����äƤ뤫��虜�虜����ʥ����ɤ����롣

     �ʤ� int main(...... ��ʤ����⤷��ؤ󤷤Τ���

     mac.c
     win.c �⤢��󤫤ʡ� mac��win�ϥ��������DblClick�����Ǥ���ʡ�

     �֤פ����פΥ����ɤι�¤
     
     ��åѡ��ץ���ब �������ƥ����㤴�Ȥˤ��롣
     * mac.c , win.c , linux.c
     �����Ĥ�Τ����ҤȤĤ򤨤��ǡ������
     proxy??.c ����������ѥ��뤹��ȤǤ�������Ȥ��롣

     �ե������ʬ�䤹��ɬ�פ��ǤƤ����顢 mac??.c �Ȥ��ˤ��Ƥ�������
     ��åѡ��ץ������Ѱդ���ؿ����ϡ��ߤ��Ʊ��̾���δؿ���
     ���Ѱդ�����«�ˤ��Ƥ����ʤ��ȡ�proxy??.c ����ʤ��ʤ롣
     �����ˤ��Υꥹ�Ȥ򤫤��Ȥ�����

�ޤ����� �����åȤ򤢤�魯�ϥ�ɥ�ϡ� Unix �� fd �ߤ����� int ��
�ֹ�(�ե������ɽ�ؤ�index)�ˤ��롣����򥷥ߥ�졼�Ȥ���Τϡ�
��åѡ��λŻ��� linux.c �ǤϤȤꤢ�������⤷�ʤ��Ƥ����Τǡ�
���⤷�ʤ��� linux.c �ǥ���ѥ��뤷�����ϡ������ͤϤ��֤�3��4���餤����
�Ϥ��ޤ뤱�ɡ� mac�Ȥ����� 0����Ϥ��ޤ�Τ��⤷��󡣤���Ϥ���Ǥ褤��
�ϥ�ɥ���ͼ��Τ˰�̣�Ϥʤ��������åȤ��Ȥ˰ۤʤ�Ф褤��

�֤��ͤΤ䤯����
�����Unix�Υ����ƥॳ����ݤ�����ʤ鼺�ԡ�0���������Х��ȿ��Ȥ���
int �Ǥ������ޤ��礦��

int fd = setupServerSocket( int port ) : �����С������åȤ��������롣
win.c ����winsock ����������������Ϥ���ΤǤ��礦��
mac.c ���Ȥ狼��ޤ��󡣤��ä����ɤ���äƤ��Τ��ʡ�



1   �ޤ��פ�������ư����Ȥ��ˡ����ʤ餺 win,max,linux�Τɤ�Ǥ⡢
   startProxy( int argc , char *argv ) �Ȥ����ؿ�����Ф�롣
   ���줬 main�ؿ��Υ�åѡ��Ǥ��롣argc,argv �λ��ͤϡ�unix�ˤ��碌��
   ���ȡ��ޥå��Ȥ���������ä��顢����ϵ�ư���˥���������Ф���
   ��᤿��Ȥ����硼�ȥ��åȤ���Ͽ������Ȥ�����������


 */

#include "common.h"     /* win,mac,linux���ߤ���Ѱդ�����̲��ؿ��� */
#include "proxy.h"      /* proxy �ε�ǽ�ˤĤ��Ƥ� */
#include "linux.h"      /* linux ������ */
                           

#define DEFAULTLOG  "proxy.log"


/*
 *
 * ��³��������뤿��ι�¤��
 * index �� fd �򤽤ΤޤޤĤ�����
 */
typedef enum
{
    CONTYPE_CLI = 1,        /* ���饤����Ȥ���Υ��ͥ������ */
    CONTYPE_SV,             /* �����С��ȤΥ��ͥ������ */
    CONTYPE_MAX
} CONTYPE;

typedef enum
{
    CONSTATE_HTTP_HEADER = 100, /* HTTP �Υإå����ߤ������Ƥ�����֡�*/
    CONSTATE_HTTP_BODY,         /* HTTP �Υإå��Τ��Ȥζ��ԤΤ���
                                   ���ߤ������Ƥ�����֡� */
    CONSTATE_FTP,               /* FTP �ΤȤ��� */
    CONSTATE_MAX
} CONSTATE;

#define NETBUFSIZE 65536
typedef struct
{
    int use;                /* �����椫�ɤ����Υե饰 */
    char rb[NETBUFSIZE];    /* �ɤߤ��ߥХåե��� */
    int rbuse;                       
    char wb[NETBUFSIZE];    /* �񤭤��ߥХåե��� */
    int wbuse;
    int id;                 /* socket id */
    CONTYPE type;           /* ���ͤ������μ��� */
    CONSTATE state;         /* ���ͤ������ξ��� */
    int close_flag;         /* ���Υե饰�����äƤ���ȡ��ǡ���������
                               ��λ�����������Ĥ��� */
    int asoc_id;            /* ��Ϣ�Ť����Ƥ��륵��������³��-1���ä���
                             �ޤ����������Ƥ��ʤ���*/

} Con;

#define MAXCON 128
Con con[MAXCON];            /* ����� fd */

/*
 * Ŭ���ʥᥤ��롼����
 *
 *
 *
 *
 */
int main(int argc , char **argv )
{

    /* Linux �Ǥϡ���������� �ޤä��Ȥ����Ѥ����� */
    return startProxy( argc, argv );

}

/*
 * ���ͥ������� ID ��դ롣
 *
 *
 */
static int p_createNewSocketID( void )
{
    static int s_id = 0;

    s_id ++;

    return s_id;
}

/*
  ���̲��Ѵؿ������Υץ�ȥ����פ� common.h �ˤ���

  ��������줿�����Х����åȤ򤫤���

  ���äѤ����������.����������������

  �Ȥꤢ���� ftp �ˤĤ��ƤϤۤä��餫��

  http_port ����Ϥ���뤬 bind ���顼���Ǥ���1�ŤĤդ䤷�Ƥ��᤹��
  100������äƤ�����ä��餢�����롣
 */
int p_initServerSocket( int http_port , int ftp_port )
{
    int i;
    int ret;
    int s;
    
    /* �ޤ� HTTP ���� */
    for( i = 0 ;i < 100;i++){
        
        struct sockaddr_in sin;
        
        s = socket( AF_INET , SOCK_STREAM , 0 );
        if( s < 0 ) return s;

        bzero( &sin , sizeof( sin ));
        sin.sin_port = htons( http_port + i );
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = INADDR_ANY;

        ret = bind( s , (struct sockaddr*)&sin,
                    sizeof(struct sockaddr_in ));
        if( ret < 0 ){
            close( s );
            continue;
        } else {
            break;
        }
    }
    if( i == 100 ) return -100;
    if( i > 0 ){
        char msg[100];
        sprintf( msg , "Bound on port: %d\n" , http_port + i );
        p_print( msg);
    }
    
    ret = listen( s , 5 );
    if( ret < 0 ){
        p_printError( "listen error!\n");
    }

    p_print( "server socket ok\n" );
    return s;
}
/*
 * �����С������åȤ�Ȥ��ޤ��礦�δؿ�
 */
void p_closeServerSocket( int s )
{
    close( s );
}
/*
  ���̲��Ѥ��󤹤�

  ���顼��å������������ �ޤä��Ȥ�������ʤ顢
  ���������ܥå������ʡ��ե�����˥�������Ϥɤ��ˤ����Τ�
  �ʤ���
  
 */
void p_printError( char *s )
{
    /* linux �ǤϤ��Τޤ޽Ф����ե�����ؤΥ��ϡ�
       ����ͤ��Ƥ��ʤ����ͤ���       */
    fprintf( stderr , s );

    {
        FILE *fp = fopen( DEFAULTLOG , "a" );
        if( !fp ) return;
        fprintf( fp , s );
        fclose(fp);
    }
}
/* ���顼����ʤ��ץ��� */
void p_print( char *s )
{
    p_printError( s );
}

/*
 * �ݡ���󥰤ν���
 * 
 * int sockfd : �ᥤ��� accept�ѤΤ��
 */
fd_set rfd , wfd , efd;   /* �ݡ�����ѥǡ�����¤ */
struct timeval timeout;     /* �����ॢ���Ȥλ��� */
void p_preparePolling( int sockfd )
{
    int i;
    
    FD_ZERO( &rfd );
    FD_ZERO( &wfd );
    FD_ZERO( &efd );

    timeout.tv_sec = 0;
    timeout.tv_usec = 1000*100;

    /* �ᥤ���sockfd ����������� */
    FD_SET( sockfd , &rfd );
    FD_SET( sockfd , &efd );

    /* �Ҷ������å� */
    for(i=p_getMinPollingCount();i<p_getMaxPollingCount();i++){
        if( !con[i].use ) continue;

        FD_SET( i , &rfd );
        FD_SET( i , &efd );
        if( con[i].wbuse > 0 ) FD_SET( i , &wfd );
    }

}

/*
 * Write buffer �� ���դȤ���
 *
 */
static int p_shiftWriteBuffer( int s , int len )
{
    int cp;
    if( con[s].wbuse <= len ){
        cp = con[s].wbuse ;
    } else {
        cp = len;
    }
    
    memmove( con[s].wb , con[s].wb + cp , con[s].wbuse - cp );
    
    con[s].wbuse -= cp;
    return cp;
}


/*
 * �ͤäȤ���Τ�ߤ���/�������Фäդ������ɲä���
 */
static int p_appendReadBuffer( int s , char *buf , int len )
{
    int cp;
    
    if( !con[s].use )return -1;
    if( ( con[s].rbuse + len ) >= NETBUFSIZE ){
        cp = NETBUFSIZE - con[s].rbuse;
    } else {
        cp = len;
    }
    memcpy( con[s].rb + con[s].rbuse , buf , cp );
    con[s].rbuse += cp;
    
    return cp;
}
static int p_appendWriteBuffer( int s, char *buf , int len )
{
    int cp;
    if( !con[s].use )return -1;
    if( ( con[s].wbuse + len ) >= NETBUFSIZE ){
        cp = NETBUFSIZE - con[s].wbuse;
    } else {
        cp = len;
    }
    memcpy( con[s].wb + con[s].wbuse , buf , cp );
    con[s].wbuse += cp;
    return cp;
}


/* close it */
static void p_closeSocket( int s )
{
    close(s);
    con[s].use = con[s].rbuse = con[s].wbuse = 0;
    con[s].asoc_id = con[s].id = -1;
    fprintf( stderr,"closed socket\n");
}

/*
 * �ºݤ˥ݡ���󥰤���
 *
 * int sockfd : �ᥤ��� socket fd
 */
#define RWUNIT   1024

void p_doPolling( int sockfd )
{
    int r,i;

    r = select( p_getMaxPollingCount() + 1, &rfd , &wfd , &efd, &timeout );

    /* accept �⤷�Ƥޤ��� */

    if( FD_ISSET( sockfd , &rfd )){
        int newsockfd;
        int addrlen = sizeof( struct sockaddr_in );
        struct sockaddr_in sin;
        
        newsockfd = accept( sockfd ,  (struct sockaddr*)&sin,
                            &addrlen );
        if( newsockfd < 0 ){
            p_printError( "accept error\n");
        } else if( con[newsockfd].use ){
            p_printError( "con array inconsistency\n");
        } else {
            /* ����͡� */
            con[newsockfd].use = 1;
            con[newsockfd].rbuse = con[newsockfd].wbuse = 0;
            /* accept���ƤǤ�����Ĥϡ����饤����ȤȤ���³�ѤʤΤ��� */
            con[newsockfd].type = CONTYPE_CLI;
            con[newsockfd].id = p_createNewSocketID();
            con[newsockfd].state = CONSTATE_HTTP_HEADER;
            con[newsockfd].close_flag = 0;
            con[newsockfd].asoc_id = -1;
        }
    }


    for(i=p_getMinPollingCount();i<p_getMaxPollingCount();i++){
        if( !con[i].use )continue;
        
        /* �ޤ� read */
        if( FD_ISSET( i , &rfd )){
            int r;
            char buf[RWUNIT];
            

            r = read( i , buf , sizeof( buf ) );
            if( r <= 0 ){

                /* �����ǻ�����ե饰��������Τǡ����Τ��ȥ꡼�ɤ��褦
                   �Ȥ��Ƥ⡢���顼�ˤʤ�Τǡ������ǤΥ��顼������ɬ�פʤ�*/
                fprintf( stderr , "read error\n");
                if( !p_isClientSocket(i)){
                    fprintf( stderr , "close asociated client\n");
                    p_setCloseFlag( p_getAssociatedSocket(i));
                }
                p_closeSocket( i );

                continue;
            }
            p_appendReadBuffer( i , buf , r );
        }
        if( FD_ISSET( i , &wfd )){
            int w,tobe;
            
            if( con[i].wbuse >= RWUNIT ) tobe = RWUNIT;
            else tobe = con[i].wbuse;
            
            w = write( i , con[i].wb , tobe );
            if( w <= 0 ){
                p_closeSocket(i);
                continue;
            }
            p_shiftWriteBuffer( i , w );

            /* Close flag �����äƤ�����Ȥ��� */
            if( con[i].wbuse == 0 && con[i].close_flag ){
                p_closeSocket(i );
                continue;
            }
        }
        if( FD_ISSET( i , &efd )){
            /* �ʤˤ⤷�ʤ����Ȥꤢ���� */
        }
    }
}

/*
 * �ݡ���󥰤ν�λ�������ʤˤ⤷�ʤ���
 */
void p_finishPolling( int s  )
{
    ;
}


/*
 * �Ԥ��������Ƥ뤫�ɤ�������٤롣
 * �ǽ餫�鸡������'\n' �����ä��� 1�򤫤���
 *
 * ��������� -1 �ʤ鴰�����Ƥʤ��� 0�ʾ�ʤ顢���ΰ��֤˲��Ԥ����롣
 */
static int p_completeLine( int fd )
{
    int i;
    char *s = con[fd].rb;
    int len = con[fd].rbuse;

    for(i=0;i<len;i++){
        if( s[i] == '\0' ) return -1;
        if( s[i] == '\n' ) return i;
    }
    return 0;
}
/* �إå����������Ƥ���1�򤫤������������Ƥʤ�����0 */
static int p_completeHeader( int fd )
{
    int i;
    char *s = con[fd].rb;
    int len = con[fd].rbuse;

    if( len <= 1 ){
        return 0;
    }
    for(i=0;i<len-1;i++){
        if( s[i] == '\n' &&
            s[i+1] == '\n' ) return i+1;
    }
    return 0;
}

/*
 * ���̲����줿�����åȤ����ɤߤ���
 *
 */
int p_socketRead( int s , char *buf , int maxlen )
{
    int cp;


    if( !con[s].use ) return -1;
    
    if( con[s].rbuse == 0 ) return 0;
    
    if( con[s].rbuse <= maxlen ){
        cp = con[s].rbuse ;
    } else {
        cp = maxlen;
    }
    
    memcpy( buf , con[s].rb , cp );
    memmove( con[s].rb, con[s].rb + cp , con[s].rbuse - cp );
    
    con[s].rbuse -= cp;

    return cp;
}

/*
 * \r������򤫤����Ĥ��롣 \r�򤼤�ֺ�����ƥ��եȤ��롣
 * readline�ʳ��ΤȤ��ˤ�ä���Х��ʥ꤬�����뤫�������衣
 */
static void p_compressLine( int fd )
{

    int src_i = 0 , dest_i = 0;
    int modcount = 0;
    char *s = con[fd].rb;
    int len = con[fd].rbuse;
    
    while(1){
        s[dest_i] = s[src_i];
        
        if( s[src_i] == '\0' )break;
        if( s[src_i] == '\r' ){ dest_i += 0; modcount++;}else dest_i ++;
        src_i ++;
        if( src_i == len )break;
    }
    
    s[dest_i]='\0';
    con[fd].rbuse -= modcount;
}

/*
 * ���̲����줿�����åȤ���1�Ԥ�ߤ��ࡣ�Ԥ��ʤ����ɥХåե��ˤϤ��ޤäƤ�
 * ���ϡ��ʤˤ��ߤ��ޤʤ���HTTP�إå����ɤ�ΤˤĤ�����
 * �֤��ͤϡ���ߤ�����Х��ȿ���
 *
 * '\n\r' �� '\n' �� '\r\n' �⡢����� '\n' ���Ѵ����Ƥ�������
 * maxlen ��1�ΤȤ��Ǥ⡢  �嵭���Ѵ��򤹤�褦�ˡ�1ʸ��;ʬ��
 * �Хåե��������Ѱդ��롣
 *
 *��������� -1: ���顼
             -2: �Хåե����ߤ��������ơ�1�Ԥ�ߤ���ʤ��ä��ΤǤ�����᤿��
 */
int p_socketReadLine( int s , char *buf , int maxlen )
{

    int newline_index;
    int r;
    
    if( !con[s].use )return -1;
    if( con[s].rbuse == 0 )return 0;
    
    /* �ޤ���1�Ԥ��������Ƥ뤫�ɤ�������٤� */
    if( (newline_index = p_completeLine( s )) <= 0 )
        return 0;

    /* \r ������򤳤��Ǥ������� */
    p_compressLine( s );
    

    /* �ɤ��˲��Ե��椬����Τ������Ѥ��ơ� */
    if( newline_index >= maxlen ) return -2;

    r = p_socketRead( s , buf , newline_index );

    if( r > 0 ){
        buf[r]='\0';       /* �̤�⤸ */
    }
    
    return r;
}

/*
 * HTTP �Υإå����ɤߤ��ࡣ�������Ƥ�Ȥ�������
 * �����Ȥϡ� compressLine�������Ȥˡ� "\n\n" ���Хåե����ˤդ��ޤ�Ƥ���
 * �Ȥ������ϤˤϤ��ζ��Ԥ�դ�����֤���
 *
 * �֤��͡� �ºݤ˥إå����ߤ�����Ф����Ϥ��ΥХ��ȿ���
 * �ޤ��������Ƥʤ��Ȥ��Ȥ���0�� ���顼���ä�����
 *
 * -1 : ���顼
 * -3 : �إå����Хåե��ˤϤ��꤭��ʤ���
 * -5 : HTTP �ǤϤʤ���
 */
int p_socketReadHTTPHeader( int s , char *buf , int maxlen )
{
    int last_newline_index;
    int r;
    
    if( !con[s].use ) return -1;
    if( con[s].rbuse == 0 ) return 0;
    if( !p_isHTTP(s)) return -5;
    
    /* \r ������򤳤��Ǥ������� */
    p_compressLine( s );
    
    /* �إå����������Ƥ뤫�ɤ�������٤롣 */
    if( ( last_newline_index = p_completeHeader(s)) <= 0 ){
        return 0;
    }

    if( last_newline_index >= maxlen ) return -3;

    r = p_socketRead( s , buf , last_newline_index );
    if( r >0 ){
        buf[r] = '\0';
    }
    return r;
}

/*
 * ���̲����줿�����åȤˤ��ä����ࡣ
 *
 * �֤��͡� ���ä����˽񤭤�����Х��ȿ�
 */
int p_socketWrite( int s , char *buf , int len )
{
    if( !con[s].use ) return -1;
    return p_appendWriteBuffer( s , buf , len );
}

/*
 * ��³�������Ĥ�������Υ���ǥ����򤫤���
 *
 */
int p_getMinPollingCount( void )
{
    return 3;       /* �ǥХå���Ĥ����Ȥ����Ѳ����뤬̵�� */
}
/* ����� index+1 ( ������ ) �򤫤��� */
int p_getMaxPollingCount( void )
{
    return sizeof( con ) / sizeof( con[0] ) ;
}

/*
 * �ۥ���̾���饢�ɥ쥹��ȤäƤ��� connect���������åȤ򤫤���
 * 
 * Windows�Ǥϡ��Υ�֥�å��󥰤�connect��Ĥ�����ˡ���狼��ʤ��ä���
 * �Τǡ��դĤ��˥֥�å�����С�������ޤ��Ĥ��äƤߤ롣
 *
 * ���������
 * -1 : gethostbyname error ( site���ߤĤ����Ȥ����� )
 * -2 : socket error
 * -3 : connect error
 * -4 : �����Ǥ��ʤ��������顼(�Ф��äƤޤ���)
 */
int p_connectToHTTPServer( char *h , int port )
{
    struct hostent* he;
    struct sockaddr_in sin;
    int sockfd;
    int r;
    
    sin.sin_family = AF_INET;
    sin.sin_port = htons( port );
    sin.sin_addr.s_addr = inet_addr( h );
    if( sin.sin_addr.s_addr == -1 ){
        he = gethostbyname( h );
        if( he == NULL ){
            p_print( "gethostbyname err." );
            return -1;
        }
        memcpy((void*)&sin.sin_addr.s_addr,
               he->h_addr, sizeof( struct in_addr));
    }

    sockfd = socket( AF_INET , SOCK_STREAM , 0 );
    if( sockfd <0 ){
        p_printError( "socket error\n" );
        return -2;
    }

    r = connect( sockfd , ( struct sockaddr*) &sin,
                 sizeof( struct sockaddr_in ));
    if( r < 0){
        p_print( "cannot connect\n");
        return -3;
    }


    
    /* �����åȤ�������Ǥ����Τǡ�����ɤϡ�Con����Ͽ���롣 */
    if( con[sockfd].use ){
        p_printError( "in p_connectToServer: Con array inconsistency\n");
        return -4;
    }
    
    fprintf( stderr , "created client socket fd%d\n",sockfd);
    
    con[sockfd].use = 1;
    con[sockfd].rbuse = con[sockfd].wbuse = 0;
    con[sockfd].type = CONTYPE_SV;
    con[sockfd].id = p_createNewSocketID();
    con[sockfd].state = CONSTATE_HTTP_HEADER;
    con[sockfd].close_flag = 0;
    con[sockfd].asoc_id = -1;
    return sockfd;
}

/*
 * �ץ�ȥ���μ���򤷤�٤�
 *
 * 0���ä��餽�Υץ�ȥ���ǤϤʤ���1���ä��餽�Τפ�Ȥ���Ǥ��롣
 */
int p_isHTTP( int fd )
{
    if( !con[fd].use )return 0;
    if( con[fd].state == CONSTATE_HTTP_HEADER ||
        con[fd].state == CONSTATE_HTTP_BODY ){
        return 1;
    } else {
        return 0;
    }
}
int p_isFTP( int fd )
{
    if( !con[fd].use )return 0;
    if( con[fd].state == CONSTATE_FTP ){
        return 1;
    } else {
        return 0;
    }
}



/*
 * Close flag ��Ĥ���
 *
 */
void p_setCloseFlag( int fd )
{
    
    if( fd < 0 || fd >= p_getMaxPollingCount() || !con[fd].use ) return;
    con[fd].close_flag = 1;
}



/*
 * �إå����Τ�ߤ��ߤ�����ä����Τˤ��äƤ��롣
 *
 */
void p_startHTTPBody( int s )
{
    con[s].state = CONSTATE_HTTP_BODY;
}

int p_isHTTPHeaderState( int s )
{
    return ( con[s].state == CONSTATE_HTTP_HEADER );
}

int p_isClientSocket( int s )
{
    return ( con[s].type == CONTYPE_CLI );
}

/*
 * ��³���ڤ�Ƥ뤫�ɤ�������٤�
 *
 */
int p_isClosed( int s )
{
    int sa;
    /* ���̤Ϥ���� OK���Ǥ⡢���ʤ�fd������Ѥ���
       ���顢���ξ�郎�ҤĤ褦�ˤʤ�Τ��� */
    if( s < 0 || s >= p_getMaxPollingCount() ) return 0;
    if( con[s].use == 0 )return 1;

    /* ���θ����ΤȤ����ߤ�Ф狼�뤬�� �������id�����פ����Τ���
       �򸡺�����Τǡ���Ū��ã�������Τ��� */
    sa = p_getAssociatedSocket(s);
    if( sa < 0 ){
        return 1;
    }

    return 0;
}
/*
 * 2�ܤ���³���Ϣ�Ť��롢���������
 *
 * int c : client connection
 * int s : server connection
 */
void p_associateSocket( int c , int s )
{
    con[c].asoc_id = con[s].id;
    con[s].asoc_id = con[c].id;
    fprintf(stderr,"Associated fd%d(%d) <->  fd%d(%d)\n",
            c,con[c].id , s,con[s].id);
}
int p_getAssociatedSocket( int so )
{
    int i;
    for(i=p_getMinPollingCount();i<p_getMaxPollingCount();i++){
        if( con[so].asoc_id == con[i].id &&
            con[i].use && con[so].asoc_id >=0 ){
            return i;
        }
    }
    return -1;
}
