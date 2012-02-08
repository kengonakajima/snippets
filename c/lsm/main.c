#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <strings.h>
#include <macros.h>
#include <ulimit.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define TRUE 1
#define FALSE 0
#define UDP 0
#define TCP 1
#define VERSION "0.1"
#define PROG    "lssendmesg"
#define DEFAULTPORT 9009
#define TEST 0
#define BUFSIZE 65536


#define OPTIONSTRING "svtS:P:hdT1@:"
int waittime;


void print( const char* format , ... )
{
    va_list ap;
    va_start( ap , format );
    vfprintf( stderr ,format , ap );
    va_end(ap);
}


/****************************
  書いて読む
  成功すると 返り値は 読み込んだバイト数で recvbuf に値が入る。
  失敗すると、-1 が返る。
****************************/
int sendRecv( int sendsock,struct sockaddr_in* to, char* sendbuf,
              int sendlen , char* recvbuf ,int recvlen)
{
    int tolen= sizeof( struct sockaddr_in );
    struct timeval timeout;

    //4秒ぐらいにしようかな
    timeout.tv_usec = 0;
    timeout.tv_sec  = waittime;



    //書く
{
    fd_set writefdset;
    int ret;
    
    FD_ZERO( &writefdset );
    FD_SET( sendsock , &writefdset );

    //書けるかどうかだけの判断
    select( ulimit(UL_GDESLIM),(fd_set*)NULL,&writefdset,(fd_set*)NULL,NULL);
    if( FD_ISSET( sendsock , &writefdset ) ){
        ret = sendto(sendsock,sendbuf,sendlen,0,(struct sockaddr*)to,tolen );
        if( ret  == -1 )return -1;
    }
}




//読む
{
    int comelen=0;
    int rc;
    fd_set readfdset;
    FD_ZERO( &readfdset );
    FD_SET( sendsock , &readfdset );
    
    //読み込めるまで or 時間まで待つ
    rc = select( ulimit(UL_GDESLIM), &readfdset,(fd_set*)NULL,(fd_set*)NULL,&timeout);

    if( rc == 0 ){
        printf( "error");
        //タイムアウトだ。
        return comelen;
    }

    if( FD_ISSET( sendsock , &readfdset ) ){
        struct sockaddr from;
        int fromlen = sizeof( struct sockaddr );
        int ret;
        ret = recvfrom(sendsock,recvbuf+comelen,recvlen-comelen,0,&from,&fromlen);
        if( ret == -1 ){
            printf( "error");
            return -1;
        }
        return fromlen;
    }
}
return -1;
}






int makesockaddr_in( struct sockaddr_in* s_in , char* servername , int port )
{
    struct hostent* serverent;
    serverent = gethostbyname( servername );
    if( serverent == NULL ){
        print( "Can't get hostent: %s\n" , servername );
        return FALSE;
    }
    s_in->sin_family = AF_INET;
    s_in->sin_port   = htons( port );
    s_in->sin_addr = *(struct in_addr*)serverent->h_addr;

    printf( "PORT:%d ADDR: %x\n", port , (int) serverent->h_addr);
    return TRUE;
}


int makeUdpSocket( int port )
{
    int s, ret;
    struct sockaddr_in sin;

    s = socket( AF_INET , SOCK_DGRAM ,0 );
    if(s == -1)return -1;

    bzero( &sin , sizeof( struct sockaddr_in ) );
    sin.sin_family = AF_INET;
    sin.sin_family = htons( port );
    sin.sin_addr.s_addr = INADDR_ANY;

    ret = bind(s, (struct sockaddr*)&sin ,sizeof( struct sockaddr_in ));
    if( ret == -1 )return -1;

    return s;
}



int makeTcpSocket( int port )
{
    int s, ret;
    struct sockaddr_in sin;

    s = socket( AF_INET , SOCK_STREAM ,0 );
    if(s == -1)return -1;

    bzero( &sin , sizeof( struct sockaddr_in ) );
    sin.sin_family = AF_INET;
    sin.sin_port = htons( port );
    sin.sin_addr.s_addr = INADDR_ANY;

    ret = bind(s, (struct sockaddr*)&sin ,sizeof( struct sockaddr ));
    if( ret == -1 )return -1;

    ret = listen( s , 100);
    if( ret == -1 )return -1;

    return s;
}



#define usage  print( "usage: lssendmsg [" OPTIONSTRING "] command\n" )

void showHelp( void )
{
    usage;
    print( "\t[-h]\t\t\tshow this help\n" );
    print( "\t[-v]\t\t\tshow version\n" );
    print( "\t[-s]\t\t\tserver mode\(default udp port 9009)\n" );
    print( "\t[-t]\t\t\tspecify server use tcp(this option \n\t\t\t\t\tmakes sense when -s is speicfied\n" );
    print( "\t[-T]\t\t\tsend Test command( send 0xfa 0x64 )\n" );
    print( "\t[-d]\t\t\tdebug mode\n" );
    print( "\t[-S servername]\t\tspecify server name(default value is localhost)\n" );
    print( "\t[-P port\t\t\tspecify server port(default value is 9000)\n" );
    print( "\t[-1]\t\t\tset waittime 2\n" );

}


int convertbuffer( char* buf, int readlen , char* sendbuf )
{
    int i;
    for(i = 0 ; i < readlen ; i ++ ){
        if( buf[i] == ' ')
            sendbuf[i] = '\0';
        else
            sendbuf[i] = buf[i];
    }
    return i;
}

int main( int argc, char** argv )
{
    extern char* optarg;
    extern int optind ,opterr;
    int c;

    /*サーバーモードで起動するかどうか 9009番で待つ */
    int servermode = FALSE;
    int tcporudp = UDP;

    char servername[64];        //サーバの名前
    int  port=9000;             //サーバのポート

    int clisock;                //クライアント用のソケット
    struct sockaddr_in toserver;    //サーバの sockaddr_in

    int testmode = FALSE;
    int debugmode = FALSE;

    waittime = 4;
    bzero( &toserver , sizeof(struct sockaddr_in ) );
    strcpy( servername ,"localhost" );

    while( ( c =  getopt( argc, argv, OPTIONSTRING ) ) != -1 )//"svtS:P:hdT1" ) ) != -1 )
        switch( c ){
        case 's':
            servermode = TRUE;
            break; 
        case 'v':
            print( PROG "\t" VERSION "\n");
            exit( 0 );
            break;
        case 't':
            tcporudp = TCP;
            break;
        case 'S':
            strcpy( servername ,optarg );
            break;
        case 'T':
            testmode = TRUE;
            break;
        case 'd':
            debugmode = TRUE;
            break;
        case '1':
            waittime = 1;
            break;
        case '@':
        {
            char *buf;
            waittime = strtol( optarg, &buf, 10);
            if( strlen( buf ) >= 1 ){
                print( "Input digit number\n");
                exit( 0 );
            }
            break;
        }
        case 'P':
        {
            char *buf;
            port = strtol( optarg, &buf, 10);
            if( strlen( buf ) >= 1 ){
                print( "Input digit number\n");
                exit( 0 );
            }
            break;
        }
        case 'h':
            showHelp();
            exit(0);
        default:
            usage;
            break;
        }

    if( debugmode ){
        DEBUG( servername, s);
        DEBUG( port,d);
    }


    print( ".");
    /* 送り先用の sockaddr_in を作る */
    if( makesockaddr_in(&toserver, servername, port) == FALSE )
        exit(1);
    print( ".");
        
    /* 送り用udp socket を作る */
    clisock = makeUdpSocket(0);     /* 適当なアドレスにバインドしてもらう */
    if( clisock == -1 )      exit( 1 );
    print( ".");

    if( servermode ){

        if( tcporudp == UDP ){
            //サーバ用のソケットを作る
            int s;
            s = makeUdpSocket( DEFAULTPORT );
            if( s == -1 ) exit( 0 );

/*            while( 1 ){
              read();
              write();
            }    
*/
        }else{

            //サーバ用のソケットを作る
/*            int s;
            s = makeTcpSocket( DEFAULTPORT );
            if( s == -1 ) exit( 0 );
            DEBUG( s , d) ;

            while( 1 ){
                struct sockaddr_in sin;
                int len = sizeof( struct sockaddr );
                char  buf[BUFSIZE];
                int     nsock;
                int     readlen;
                int     sendlen;
                char    sendbuf[BUFSIZE];
                char    recvbuf[BUFSIZE];
                int     length = 0;
                int     ret;

                print( "aho\n" );
                nsock = accept( s ,(struct sockaddr*)&sin , &len) ;
                print( "aho\n" );
                readlen = read( nsock , buf , BUFSIZE );
                if( readlen <= 0 ){
                    DEBUG( readlen , d );
                    close( nsock );
                    continue;
                }
                //
                sendlen = convertbuffer( buf, readlen , sendbuf );
                ret = sendRecv( clisock, &toserver,sendbuf ,length, recvbuf , BUFSIZE );
                if( testmode )
                    printf( "%s\n" , &recvbuf[0] );
                else if( ret >= 1 )
                    printf( "%s\n" , &recvbuf[1] );

                {
                    char toclibuf[BUFSIZE];
                    sprintf( toclibuf,  "%s\n" ,  &recvbuf[1] );
                    write(nsock , toclibuf , strlen( toclibuf ) );
                }

                close( nsock );
            }

            */
        }
    }else{

        char    sendbuf[BUFSIZE];
        char    recvbuf[BUFSIZE];
        int     length = 0;
        int     ret;

        if( !testmode  && optind == argc  )
            /* 何もインプットがない */
            exit( 0 );

        //最初の内容
        sendbuf[0] = 0xa1;
        length = 1;

        for( ; optind < argc ; optind ++ ){
            int addlen = strlen( argv[optind] );
            strcpy( sendbuf+length , argv[optind] );
            length += addlen + 1;
        }

        if( testmode ){
            sendbuf[0] = 0xfa;
            sendbuf[1] = 0x64;
            length = 2;
        }


        if( debugmode ){
            int i;
            for( i = 0 ; i < length ; i ++){
                char put = sendbuf[i];
                DEBUG( put, x);
            }
            DEBUG( length , d);
        }

        ret = sendRecv( clisock,&toserver,sendbuf ,length, recvbuf , BUFSIZE );
        if( testmode )
            printf( "%s\n" , &recvbuf[0] );
        else if( ret >= 1 )
            printf( "%s\n" , &recvbuf[1] );

        close( clisock );
    }
    exit( 0 );
}
