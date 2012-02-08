/*
  connector









  */

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
#include <signal.h>
#include <sys/signal.h>

int connect_close( char *host , int port );
double time_diff(struct timeval subtrahend,
                       struct timeval subtractor);
int getAddr( char **argv , char *adr );
int getLogfile( char **argv, char *out );
int getLoopCount( char **argv );
int isServerMode( char **argv );
int getPort( char **argv );


void doClient(void);
void doServer(void);

void inthandler(int );

/* �ǥե���ȤǤϥ��饤����ȡ�
   -s ��Ĥ���ȡ�INADDR_ANY �Υ����Фˤʤ�
   -l 100 ��100�󤯤꤫�������ǥե���Ȥ�0xffffffff��
   -noclose ��close���ʤ��������ФΥ������ˤޤ����롣
   -a 192.168.1.62
   -p 80 �ݡ��ȡ�
   -log outfile ���ե��������
 */
/* ξ���ǤĤ��� */
struct timeval start_time,end_time;
int server_mode = 0;
unsigned int loop_count = 0xffffffff;
int noclose = 0;
int port , defaultport = 80;
char ipaddr[128];
int actual_loop = 0;
extern int errno;
int last_errno;
char logfile[1024];
FILE *fp = NULL;

/* ���饤������Ѥ����� */
int cli_successful = 0;
int cli_socket_fail = 0;
int cli_connect_fail = 0;
int cli_close_fail = 0;






int main( int argc , char **argv )
{
    int ret;
    
    signal( SIGINT , inthandler );

    if( isServerMode( argv ) ) server_mode =1;

    /* �롼�ײ�������ꤵ�줿���񤭤��� */
    ret = getLoopCount( argv );
    if( ret > 0 ) loop_count = ret;

    /* ���ɥ쥹 */
    strcpy( ipaddr , "127.0.0.1" );
    if( !getAddr( argv , ipaddr ) ){
        fprintf( stderr , "use localhost as a remote host \n");
    }
    /* �ݡ��� */
    port = getPort( argv );
    if( port < 0 ){
        port = defaultport;
        fprintf( stderr, "please specify port number. use default:%d\n",
                 port );
    }
    /* ���ե����� */
    if( getLogfile(argv,logfile)){
        fprintf( stderr , "print log into file %s\n" , logfile );
    }
    
    
    if( server_mode ){
        /* �����С��� */
        doServer();
    } else {
        /* ���饤����Ȥ� */
        gettimeofday( &start_time , NULL );        
        doClient();
    }
}

void inthandler( int i )
{
    gettimeofday( &end_time , NULL );        
    fprintf( stderr , "got SIGINT. Put stat and exit...\n");
    printf( "client result: OK:%d Esocket:%d Econnect:%d Eclose:%d\n",
            cli_successful , cli_socket_fail ,
            cli_connect_fail , cli_close_fail );
    printf( "elapsed time: %f sec ( %f sec/con)\n",
            time_diff( end_time , start_time ) ,
            time_diff(end_time,start_time)/actual_loop);

    exit(0);
}


void doServer( void )
{
}


void doClient(void)
{

    int r;

    int div10;

    if( loop_count == 0xffffffff ){
        div10 = 1000;
    } else {
        div10 = loop_count / 10 + 1;
    }
    if( logfile[0]){
        fp = fopen( logfile , "w");
        if( !fp ){
            fprintf( stderr , "cannot open file %s\n", logfile);
            return;
        }
    }
    
        
    for(actual_loop=0;actual_loop<loop_count; actual_loop++){
        r = connect_close( "192.168.1.62" , port );
        switch(r){
        case 0: {
            cli_successful ++;
            if( fp )fprintf( fp , "." );
            break;
        }
        case -1: {
            cli_socket_fail++;
            if( fp )fprintf( fp , "Esok:%s\n",strerror(errno) );
            break;
        }
        case -2: {
            cli_connect_fail++;
            if( fp ) fprintf( fp , "Econ:%s\n",strerror(errno));
            break;
        }
        case -3: {
            cli_close_fail++;
            if( fp ) fprintf( fp , "Eclose:%s\n" , strerror( errno));
            break;
        }
        default:
            break;
        }
        if( (actual_loop % div10 ) == (div10-1) )fprintf( stderr,".");
    }

        
}

/*
  ���ｪλ�� sockfd ���֤���
  ���顼�ξ�����

  ���δؿ��������˵��äƤ��ʤ��褦�ʤȤ����Ĥޤ�
  LAN�ʳ��Ǥϡ����Υƥ��Ȥΰ�̣���ʤ���
  
  char *host : dot notation 

  return -1 : socket() error
  return -2 : connect() error
  return -3 : close() error
 */
int connect_close( char *host , int port )
{
    int fd,r;
    struct sockaddr_in sockaddr;
    last_errno = 0;
    
    memset( &sockaddr , 0 , sizeof( struct sockaddr_in ));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons( port );
    sockaddr.sin_addr.s_addr = inet_addr( host );
    fd = socket( AF_INET , SOCK_STREAM , 0 );
    if( fd < 0 ){ last_errno = errno; return -1;}
    r = connect( fd , (struct sockaddr*)&sockaddr,sizeof(struct sockaddr_in));
    if( r< 0 ){ last_errno = errno;  return -2; }
    r = close( fd );
    if( r < 0 ){ last_errno = errno; return -3; }
    return 0;
}
    
/* �����С��⡼�ɤˤ��뤫�ɤ���Ƚ�ꤹ��ʤ� */
int isServerMode( char **argv )
{
    int i;
    for(i=0;;i++){
        if( argv[i] == NULL ) break;
        if( argv[i][0] == '-' &&
            argv[i][1] == 's' )return 1;
    }
    return 0;
}
/* ��������롼�ײ����������� -l 30 �Ȥ��� */
int getLoopCount( char **argv )
{
    int i;
    for(i=0;;i++){
        if( argv[i] == NULL || argv[i+1] == NULL )break;
        if( argv[i][0] == '-' &&
            argv[i][1] == 'l' ){
            return atoi( argv[i+1] );
        }
    }
    return -1;
}
/* IP���ɥ쥹,�ݡ��Ȥ򥲥å� */
int getAddr( char **argv , char *adr )
{
    int i;
    for(i=0;;i++){
        if( argv[i] == NULL || argv[i+1] == NULL )break;
        if( argv[i][0] == '-' &&
            argv[i][1] == 'a' ){
            strcpy( adr , argv[i+1] );
            return 1;       /* ���ꤷ����1������ */
        }
    }
    return 0; 
}

int getPort( char **argv)
{
    int i;
    for(i=0;;i++){
        if( argv[i] == NULL || argv[i+1] == NULL )break;
        if( argv[i][0] == '-' &&
            argv[i][1] == 'p' ){
            return atoi( argv[i+1] );
        }
    }
    return -1; 
}
int getLogfile( char **argv, char *out )
{
    int i;
    for(i=0;;i++){
        if( argv[i] == NULL || argv[i+1] == NULL )break;
        if( argv[i][0] == '-' &&
            argv[i][1] == 'l' &&
            argv[i][2] == 'o'  ){
            strcpy(out , argv[i+1]);
            return 1;
        }
    }
    return 0; 
}



double time_diff(struct timeval subtrahend,
                       struct timeval subtractor)
{
    return( (subtrahend.tv_sec - subtractor.tv_sec)
            + (subtrahend.tv_usec
            - subtractor.tv_usec  ) / (double)1E6 );
}
