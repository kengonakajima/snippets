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



int sock;




void get_socket(void);
void p( ipaddr_t ip , int pass );
unsigned short in_checksum(unsigned short *buf, int len);

int main( int argc , char **argv )
{
    struct in_addr inaddr;
    ipaddr_t ip;


        
    get_socket();

    /* initialize ip address */
    bzero( &inaddr , sizeof( inaddr ));
    if(inet_aton( argv[1] , &inaddr )<0){
        perror( "inet_aton fucked" );
        exit(1);
    }
    

    ip = inaddr.s_addr;

    p( ip , 100 );
    
    
}

void get_socket(void)
{
    sock = socket( AF_INET , SOCK_RAW , IPPROTO_ICMP );
    if( sock < 0 ){
        perror( "icmp socket" );
        exit(1);
    }

    
}



void p( ipaddr_t ip , int pass )
{
    int i , cc;
    struct sockaddr_in toaddr , fromaddr;

    /* outgoing/incoming packet buffer */
    unsigned char opacket[2048];
    unsigned char ipacket[2048];

    bzero( (char*) &toaddr , sizeof( toaddr ));
    toaddr.sin_family = AF_INET;
    toaddr.sin_addr.s_addr = ip;
    toaddr.sin_port = 0;

#define ICMP_MINLEN 8
#define HDRLEN ICMP_MINLEN      /* (8) */

    /* ICMP_ECHO , ICMP_ECHOREPLY */
    
    {
        struct icmp *icp;
        int len , fromlen;

        
        len = HDRLEN + 0;

        icp = ( struct icmp *) opacket;
        icp->icmp_type = ICMP_ECHO;
        icp->icmp_code = 0;
        icp->icmp_cksum = 0;
        icp->icmp_id = getpid() % 65536;
        icp->icmp_seq = 100;
        icp->icmp_cksum = in_checksum((u_short*)icp , len );

        cc = sendto( sock , (char*)opacket , len , 0 ,
                     (struct sockaddr*) &toaddr , sizeof( toaddr ));
        if( cc < 0  || cc != len ){
            perror( "sendto" );
            /* something wrong when seding to. */
        }

        fcntl( sock , F_SETFL , FNDELAY );

        while(1){
/*            usleep(10*1000);*/
            fprintf( stderr , "." );

            len = sizeof( ipacket );
            fromlen = sizeof( fromaddr );
            cc = recvfrom( sock , (char*)ipacket  , len , 0 ,
                           (struct sockaddr*) & fromaddr, &fromlen );
            if( cc <= 0 ){
                fprintf( stderr , "R");
            } else {
                printf( "success:len:%d",cc );
                break;
            }
        }
    }

    /* let's check the incoming packet */
    {
        struct icmp *icp;
        struct ip *ip;
        int iphdrlen ;
        
        ip = ( struct ip * ) ipacket;
        iphdrlen = ip->ip_hl << 2;

        if( ip->ip_p != IPPROTO_ICMP ){
            fprintf( stderr, "not icmp\n" );
        } else {
            fprintf( stderr , "ICMP reply!\n");
        }

        if( cc < (iphdrlen + ICMP_MINLEN) ){
            fprintf( stderr , "too short message: cc:%d iphdrlen:%d MIN:%d\n",
                     cc , iphdrlen , ICMP_MINLEN );
        } else {
            fprintf( stderr , "OK length\n" );            
        }

        icp = (struct icmp *) ( ipacket + iphdrlen );
        cc -= iphdrlen;

        if( icp->icmp_type != ICMP_ECHOREPLY ){
            fprintf( stderr , "NOT ECHOREPLY\n" );
        } else {
            fprintf( stderr , "ECHOREPLY!\n" );
        }

        if( icp->icmp_id != (getpid() % 65536 )){
            fprintf( stderr , "bad ident\n" );            
        } else {
            fprintf( stderr , "IDENT OK!\n" );
        }

        fprintf( stderr, "SEQ:%d\n" , icp->icmp_seq );
        
    }
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

