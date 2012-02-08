/*
  icmp test
  
 */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <netdb.h>

#include <sys/types.h>      /* not always automatically included */
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/file.h>
#if 1
#include <sys/ioctl.h>      /* needed for TIOCGWINSZ */
#endif

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#if 0
#include <netinet/ip_var.h> /* only needed for MAX_IPOPTLEN */
#endif



#include <netinet/ip_icmp.h>


#include <netinet/tcp.h>
#include <netinet/udp.h>

typedef unsigned int ipaddr_t;


#if defined(linux)

#include <endian.h>     /* to get the proper BYTE_ORDER */

#if !defined(BYTE_ORDER) || (BYTE_ORDER != BIG_ENDIAN && \
    BYTE_ORDER != LITTLE_ENDIAN && BYTE_ORDER != PDP_ENDIAN)
    error "Undefined or invalid BYTE_ORDER";
#endif
    

#define IPVERSION   4


#ifndef icmp_pptr
    
struct icmp {
    unsigned char icmp_type;
    unsigned char icmp_code;
    unsigned short icmp_cksum;
    union {
        unsigned char ih_pptr;
        struct in_addr ih_gwaddr;
        struct ih_idseq {
            n_short icd_id;
            n_short icd_seq;
        }ih_idseq;
        int ih_void;
    }icmp_hun;

#define icmp_pptr   icmp_hun.ih_pptr
#define icmp_gwaddr icmp_hun.ih_gwaddr
#define icmp_id     icmp_hun.ih_idseq.icd_id
#define icmp_seq    icmp_hun.ih_idseq.icd_seq
#define icmp_void   icmp_hun.ih_void
    union {
        struct id_ts {
                        n_time its_otime;
                        n_time its_rtime;
                        n_time its_ttime;
        } id_ts;
        struct id_ip {
                        struct ip idi_ip;
            /* options and then 64 bits of data */
        } id_ip;
                u_long  id_mask;
                char    id_data[1];
    } icmp_dun;
#define icmp_otime  icmp_dun.id_ts.its_otime
#define icmp_rtime  icmp_dun.id_ts.its_rtime
#define icmp_ttime  icmp_dun.id_ts.its_ttime
#define icmp_ip     icmp_dun.id_ip.idi_ip
#define icmp_mask   icmp_dun.id_mask
#define icmp_data   icmp_dun.id_data
};

#endif


#define ICMP_MINLEN 8       /* abs minimum */

/*
 * Definition of icmp type and code field values.
 */

#define ICMP_ECHOREPLY      0       /* echo reply */
#define ICMP_UNREACH        3       /* dest unreachable, codes: */
#define     ICMP_UNREACH_NET    0       /* bad net */
#define     ICMP_UNREACH_HOST   1       /* bad host */
#define     ICMP_UNREACH_PROTOCOL   2       /* bad protocol */
#define     ICMP_UNREACH_PORT   3       /* bad port */
#define     ICMP_UNREACH_NEEDFRAG   4       /* IP_DF caused drop */
#define     ICMP_UNREACH_SRCFAIL    5       /* src route failed */
#define ICMP_SOURCEQUENCH   4       /* packet lost, slow down */
#define ICMP_REDIRECT       5       /* shorter route, codes: */
#define     ICMP_REDIRECT_NET   0       /* for network */
#define     ICMP_REDIRECT_HOST  1       /* for host */
#define     ICMP_REDIRECT_TOSNET    2       /* for tos and net */
#define     ICMP_REDIRECT_TOSHOST   3       /* for tos and host */
#define ICMP_ECHO       8       /* echo service */
#define ICMP_TIMXCEED       11      /* time exceeded, code: */
#define     ICMP_TIMXCEED_INTRANS   0       /* ttl==0 in transit */
#define     ICMP_TIMXCEED_REASS 1       /* ttl==0 in reass */
#define ICMP_PARAMPROB      12      /* ip header bad */
#define ICMP_TSTAMP     13      /* timestamp request */
#define ICMP_TSTAMPREPLY    14      /* timestamp reply */
#define ICMP_IREQ       15      /* information request */
#define ICMP_IREQREPLY      16      /* information reply */
#define ICMP_MASKREQ        17      /* address mask request */
#define ICMP_MASKREPLY      18      /* address mask reply */

/*
 * Definitions needed for the udp header structure.
 */

#define uh_sport    source
#define uh_dport    dest
#define uh_ulen     len
#define uh_sum      check

#endif /* linux */





/***************************************************************/
#define DEFAULT_MAXTASK 10000
int maxtask = DEFAULT_MAXTASK;
int curmaxtask = 0;
int pid;
int icmp_sock;


#define ICMP_GIVEUP    5             /* ICMP_ECHO give up count( pkt ) */

/* struct task stands for an application level thread, which is
   equally scheduled in a infinite loop... This is the lightest thread! */


/* TT_??? is stored in type in struct task;*/
typedef enum{
    TT_PING = 10,
    TT_TCP = 11,
} TTYPE;
typedef enum{
    TS_ACTIVE = 2,       /* after sending a packet , and before receiving*/
    TS_OK = 3,              /* after receiving a reply packet. that means
                               totally OK */
    TS_TIMEOUT = 10,        /* timeout has come. should be killed */
} TSTAT;

typedef struct _task_
{
    int use;
    TTYPE type;
    TSTAT state;

    /* used by all */
    ipaddr_t remoteaddr;
    
    /* used by ICMPs */
    int id;
    int seq;

    /* used by TCPs */
    int port;

    struct timeval last_try;

    double this_time_interval;  /* sec */
}task;

task *t;


/* statistics engine */
typedef enum
{
    NOTYET = 0,
    OK = 1,
    GIVEUP = 2,
}LOG;
struct statbox
{
    unsigned char addr_a;       /* class a */
    unsigned char addr_b;       /* class b */

    
    LOG icmp_echo[65536];      /* log of icmp_echo */
    int icmp_echo_use;
    int icmp_so_far;        /* incremented from 0 to 65535 */
    double icmp_sendto_error;
    double icmp_sendto_ok;
    int icmp_echo_ok;       /* number of active hosts */
    int icmp_sendto_busy;       /* increment if EAGAIN */
    LOG tcp_23[65536];
    LOG tcp_21[65536];
    LOG tcp_25[65536];
    LOG tcp_80[65536];


};
struct statbox stb;

double get_icmp_interval(void){
    return 2.0 + 2.0 * ( (double)(random()%1000) / 1000.0  );
}
double time_diff(struct timeval newer,
                       struct timeval older )
{
    return( (newer.tv_sec - older.tv_sec)
            + (newer.tv_usec
            - older.tv_usec  ) / (double)1E6 );
}

void init_stb(int a , int b)
{
    bzero( &stb , sizeof( stb ));
    stb.addr_a = a;
    stb.addr_b = b;
    stb.icmp_so_far = 0;
}
void out_stb( void )
{
    FILE *fp;
    char fn[1024];
    
    int i,icmp_ok=0;
    for(i=0;i<65536;i++){
        if( stb.icmp_echo[i] == OK ){
            icmp_ok++;
        }
    }
    snprintf( fn , sizeof( fn ) , "%d.%d." , stb.addr_a, stb.addr_b );
    fp = fopen( fn,"w");
    fprintf( fp , "ADDR %d.%d.*.* has %d active hosts\n",
             stb.addr_a , stb.addr_b , icmp_ok );
    for(i=0;i<65536;i++){
        if( stb.icmp_echo[i] == OK ){
            fprintf( fp , "%d.%d\n",i/256 , i% 256 );
        }
    }
    fclose(fp);
}

void set_icmp_echo_log( ipaddr_t ip , LOG l )
{
    int b_c_addr = ntohl(ip) & 0xffff;
    stb.icmp_echo[b_c_addr] = l;
    if( l == OK ){stb.icmp_echo_ok ++;}
}

char * get_next_addr( void )
{
    static char ret[2048];
    snprintf( ret , sizeof( ret) , "%d.%d.%d.%d" ,
              stb.addr_a , stb.addr_b ,
              stb.icmp_so_far / 256 ,
              stb.icmp_so_far % 256 );
    stb.icmp_so_far ++;
    if( (stb.icmp_so_far % 256  ) == 0 ){
        fprintf( stderr ,"%d SENT:%6.0f ERR:%6.0f ACTIVE:%d\n",
                 stb.icmp_so_far / 256  ,stb.icmp_sendto_ok ,
                 stb.icmp_sendto_error ,stb.icmp_echo_ok );
    }
    if( stb.icmp_so_far == 65536 )return NULL;
    else return ret;
}

void send_ping( int index , ipaddr_t ip ,int id, int seq );

void init_task ( void )
{
    t = ( task * ) malloc( sizeof( task ) * maxtask );
    bzero( t , sizeof( task ) * maxtask );
}


int task_usage = 0;
int alloc_task( task *tk ) 
{
    int i;
    for(i=0;i<maxtask;i++){
        if( t[i].use == 0 ){
            memcpy( & t[i] , tk, sizeof( task ));
            t[i].use = 1;
            task_usage ++;
            return i;
        }
    }
    return -1;
}

int alloc_ping( ipaddr_t ip )
{
    task ts;
    int tind;
    
    bzero( &ts , sizeof( ts ));
    ts.type = TT_PING;
    ts.state = TS_ACTIVE;
    ts.remoteaddr = ip;
    ts.id = pid;
    ts.seq = 0;
    ts.port = 0;
    gettimeofday( &ts.last_try, NULL );
    
    tind = alloc_task( &ts );
    if( tind >= 0 ){
        t[tind].this_time_interval = get_icmp_interval();
        send_ping( tind,t[tind].remoteaddr , t[tind].id , t[tind].seq);
    }
    return tind;
}
int alloc_tcp( ipaddr_t ip , int port )
{
    task t;
    bzero( &t , sizeof( t ));
    
    t.type = TT_TCP;
    t.state = TS_ACTIVE;
    t.remoteaddr = ip;
    t.id = 0;
    t.seq = 0;
    t.port = port;
    gettimeofday( &t.last_try, NULL );
    return alloc_task( &t );
}

void get_icmp_sock( void )
{
    icmp_sock = socket( AF_INET , SOCK_RAW , IPPROTO_ICMP );
    if( icmp_sock < 0 ){
        perror( "icmp socket");
        exit(1);
    }

    fcntl( icmp_sock , F_SETFL , FNDELAY );

    fprintf( stderr ,"ICMP sockfd:%d\n", icmp_sock);
}
void kill_task( int index )
{
    /* you may make some statistics */
    bzero( & t[index] , sizeof( task ));
/*    fprintf( stderr , "killed task %d\n", index );*/
    task_usage --;
}

/* following code is taken from ping.c */

unsigned short in_checksum(unsigned short *buf, int len)
{
    register u_char *b = (u_char *)buf;
    register int n = len;       /* remaining 16-bit words */
    register int sum = 0;       /* 32-bit accumulator */
    u_short answer;

/*
 * Our algorithm is simple, using a 32 bit accumulator (sum),
 * we add sequential 16 bit words to it, and at the end, fold back
 * all the carry bits from the top 16 bits into the lower 16 bits.
 */
    while (n > 1)
    {
        answer = (b[0] << 8) | b[1];
        sum += answer;
        b += 2; n -= 2;
    }

    if (n == 1)
    {
        answer = (b[0] << 8);
        sum += answer;
    }

/*
 * Add back carry outs from top 16 bits to low 16 bits.
 */
    sum = (sum >> 16) + (sum & 0xFFFF); /* add hi 16 to low 16 */
    sum += (sum >> 16);         /* add carry */
    answer = ~sum & 0xFFFF;         /* truncate to 16 bits */
    answer = htons(answer);         /* correct order */
    return(answer);
}




/*
  construct an icmp packet and send it to the net
  */
void send_ping( int index , ipaddr_t ip ,int id, int seq )
{
    struct icmp *icp;
    struct sockaddr_in toaddr;
    int len, cc;
    char opacket[4096];
    len = ICMP_MINLEN;
    icp = ( struct icmp * ) opacket;
    icp->icmp_type = ICMP_ECHO;
    icp->icmp_code = 0;
    icp->icmp_cksum = 0;
    icp->icmp_id = id;
    icp->icmp_seq = seq;
    icp->icmp_cksum = in_checksum( (unsigned short *) icp , len );

    bzero( & toaddr, sizeof( toaddr ));
    toaddr.sin_family = AF_INET;
    toaddr.sin_port = 0;
    toaddr.sin_addr.s_addr = ip;
    
    cc = sendto( icmp_sock , (char*) opacket , len, 0 ,
                 (struct sockaddr*) & toaddr , sizeof( toaddr ));

    if( cc <0 || cc != len ){
        if( errno == EACCES ){
            /* ignore these unimportant errors */
            ;
        } else if( errno == EAGAIN ){
            stb.icmp_sendto_busy ++;
        } else {
            fprintf( stderr ,"sendto error: %d %s",errno,strerror(errno));
            stb.icmp_sendto_error ++;
        }
        return;
    } else {
        stb.icmp_sendto_ok ++;
    }
}

void proc_task( void )
{
    int i;
    struct timeval tmv;
    gettimeofday( &tmv , NULL );
    
    for(i=0;i<maxtask;i++){
        if( !t[i].use )continue;
        
        if( t[i].type == TT_PING ){
            if( t[i].state == TS_ACTIVE ){
                
                if( time_diff(  tmv , t[i].last_try ) <
                    t[i].this_time_interval )continue;

                send_ping( i,t[i].remoteaddr , t[i].id , t[i].seq);
                gettimeofday( &t[i].last_try, NULL );
                t[i].this_time_interval = get_icmp_interval();
                t[i].seq ++;
                if( t[i].seq == ICMP_GIVEUP ){
                    t[i].state = TS_TIMEOUT;
                }
            } else if( t[i].state == TS_OK ){
                set_icmp_echo_log( t[i].remoteaddr , OK );
                kill_task(i);
            } else if( t[i].state == TS_TIMEOUT ){
                set_icmp_echo_log( t[i].remoteaddr , GIVEUP );
                kill_task(i);
            }
        } else if(  t[i].type == TT_TCP ){
            if( t[i].state == TS_ACTIVE ){
            } else if( t[i].state == TS_OK ){
                kill_task(i);
            }
        }
    }
}

/*
  search a task which has the same ( ip , id , seq )
  
  you should use faster hash or something to find it,
  but that's pending ...
  
 */
void find_ping_task( ipaddr_t ip , int id , int seq )
{
    int i;
    for(i=0;i<maxtask;i++){
        if( t[i].use && t[i].type == TT_PING &&
            t[i].remoteaddr == ip &&
            t[i].id == id ){
            /* found! */
            t[i].state = TS_OK;
            return;
        }
    }

}
/*
  receive icmp packets 
 */
void recv_icmp_packets( void )
{
    char ipacket[4096];
    int len , fromlen, cc;
    struct sockaddr_in fromaddr;
    
    len = sizeof( ipacket );
    fromlen = sizeof( fromaddr );
    cc = recvfrom( icmp_sock , (char*)ipacket , len, 0 ,
                   (struct sockaddr*) &fromaddr , &fromlen );

/*    fprintf( stderr, "RECVFROM! %d\n", cc );*/
    if( cc <= 0 ){
        /* may be next time */
        return;
    } else {
        /* check if this packet is correct icmp packet or not */
        int index;
        struct icmp *icp;
        struct ip *ip;
        int iphdrlen;

        ip = (struct ip * ) ipacket;
        iphdrlen = ip->ip_hl << 2;

        if( ip->ip_p != IPPROTO_ICMP ) return;  /* discard this packet */
        if( cc < (iphdrlen + ICMP_MINLEN )) return;

        icp = ( struct icmp * ) ( ipacket + iphdrlen );
        cc -= iphdrlen;

        if( icp->icmp_type != ICMP_ECHOREPLY ) return;
            
        find_ping_task( fromaddr.sin_addr.s_addr , icp->icmp_id,
                                icp->icmp_seq );

        return;
    }
}

int add_ping( char *a )
{
    struct in_addr inaddr;
    ipaddr_t ip;
    int tid;
    
    bzero( &inaddr ,sizeof( inaddr ));
    if( inet_aton( a , &inaddr ) < 0 ) return -8;
    ip = inaddr.s_addr;
    tid = alloc_ping( ip );
    return tid;
}

/* check and create new task if possible */
int check_new_task(void)
{
    int ind;

    if( task_usage < curmaxtask ){
        int ret;
        char *p = get_next_addr();
        ret = add_ping( p );
        if( ret < 0 ){
            /* FATAL! task table is not administrated properly! */
            fprintf( stderr ,"fatal bug. task table bad admin:%d\n",ret );
            exit(1);
        }
    }
}

/*
  look at EAGAIN count within a second and increase or decrease
  scanning speed
  
 */
time_t csp_store;

void check_speed( void )
{
    time_t t;
    time(&t);
    if( t == csp_store )return;
    time(&csp_store);

    if( stb.icmp_sendto_busy > 10 ){
        /* Speed down! */
        fprintf( stderr , "D");
        curmaxtask *= 0.9;
        if( curmaxtask < 20 ) curmaxtask = 20;
    } else {
        /* Speed up! */
        fprintf( stderr , "U");
        curmaxtask *= 1.1;
        if( curmaxtask >= maxtask ) curmaxtask = maxtask;
    }
    stb.icmp_sendto_busy = 0;
}

int main( int argc , char **argv )
{
    if( argc != 3 ){
        fprintf( stderr ,"Usage: hs ADRa ADRb\n");
        exit(1);
    }
    pid = getpid() & 0xffff;

    init_task();

    get_icmp_sock();

    init_stb( atoi( argv[1] ) , atoi( argv[2]) );

    curmaxtask = 20;     /* start with this count */
    while(1){
        proc_task();
        recv_icmp_packets();
        if(check_new_task()<0)break;
        check_speed();
    }

    out_stb();
}




