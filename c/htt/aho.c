
#include <netinet/in.h>
#include <arpa/inet.h>

/* ���2�Ԥ򤱤��� winsock.h �ˤ��� */


/* �ʲ��Υ��󥯥롼�ɤϤǤ��ʤ����Ͼä� */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

int
do_http_request_forever( char *ip, unsigned short port,
                         char *obj, int sec );
    
/* �ʲ��Υᥤ��롼��������������Ƥ��� */
int
main( int argc, char **argv )
{
    while(1){
        if( do_http_request_forever(
            "203.182.168.20",80,
            "/~stoneage/newest.txt",10 ) < 0 ){
            break; 
        }
    }
}





/*
  �ʵפ�Web�˥����������ĤŤ��롣
  
  do_http_request_forever()

  static ���ѿ���Ĥ����Τǡ�����åɰ����ǤϤ���ޤ���

  char *ip : IP���ɥ쥹(�ɥå�ɽ���ο����Τߡ�̾�����٤��Τǰ����ޤ���)
  unsigned short port : �ݡ����ֹ档�դĤ���80��
  char *obj : ��������ɤ��륪�֥�������
  int sec : ���ä����ˤ�뤫

  ��������� 0�ʾ�ʤ����ｪλ����ʤ�ʤˤ����顼
 */

static int http_sock;
static int http_call_counter = 0;
static time_t http_call_last_time;
static int http_connecting = 0;
static int http_sent_request = 0;

int
do_http_request_forever( char *ip, unsigned short port,
                         char *obj, int sec )
{
    fd_set rfds,wfds;
    int r;
    struct timeval tm;

    /* ������ν����˱ƶ��򤢤����ʤ���ˡ�
       1�ä�1���Ф��褦�ˤ��� */
    if( http_call_last_time != time(NULL )){
        http_call_last_time = time(NULL);
    } else {
        return 0;
    }

    fprintf(stderr,".");
    
    if( http_call_counter == 0 ){
        int flags;
        struct sockaddr_in sin;

        /* sec�ä�1��connect���� */
        if( time(NULL) % sec != 0 )return 0;
        
        http_sock = socket( AF_INET, SOCK_STREAM, 0 );
        if( http_sock < 0 )return -1;

        fprintf(stderr,"socket()\n" );
        flags = fcntl( http_sock, F_GETFL,0);
        if( fcntl( http_sock, F_SETFL, flags|O_NONBLOCK )< 0){
            close( http_sock );
            return -2;
        }

        memset( &sin, 0 , sizeof( sin ));
        if(inet_aton(ip, &sin.sin_addr ) == 0 ){
            return -9;
        }
        sin.sin_port = htons( port );
        sin.sin_family = AF_INET;
        
        if( connect( http_sock, (struct sockaddr*)&sin,
                     sizeof(sin))<0){
            if( errno != EINPROGRESS ){
                close(http_sock);
                return -3;
            }
            http_connecting = 1;
        }
        http_call_counter ++;
        http_sent_request = 0;
        fprintf(stderr,"connected\n" );
        return 0;
    }

    FD_ZERO( &rfds );
    FD_SET( http_sock, &rfds );
    FD_ZERO( &wfds );
    FD_SET( http_sock, &wfds );    

    tm.tv_usec = tm.tv_sec = 0;
    r = select( http_sock+1, &rfds, &wfds,(fd_set*)NULL,&tm);

    if( r > 0 && FD_ISSET( http_sock, &rfds ) ){
        char buf[1000];
        r = read( http_sock, buf,sizeof(buf));
        if( r <= 0 ){
            close( http_sock );
            /* �ǽ餫����ʤ��� */
            http_call_counter = 0;
        }
        fprintf(stderr,"read %d\n",r );        
    }
    
    if( r > 0 && FD_ISSET( http_sock, &wfds ) && http_sent_request == 0 ){
        /* HTTP���׵��MSS�������˾������Τǡ�
           1���write�Ǥ��ޤ������Ȳ��ꤷ�Ƥ��롣
           �褯�ʤ������ɤ������褯�ʤ���ǽ��¸����褦�Ȥ��Ƥ���Τ�
           �����Τ��� */
        int r;
        char fuck[1000];
        sprintf( fuck, "GET %s HTTP/1.0\r\n\r\n" , obj );

        r = write( http_sock, fuck, strlen( fuck ) );

        if( r <= 0 ){
            close(r );
            http_call_counter = 0;
            return -10;
        }
        fprintf(stderr,"wrote %d\n",r );
        http_sent_request = 1;
    }
    return 0;
}





