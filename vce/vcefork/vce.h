#ifndef _VCE_H_
#define _VCE_H_

/*
  vce.h

  $Revision: 1.201 $
  $Date: 2004/01/11 15:34:01 $
  $Id: vce.h,v 1.201 2004/01/11 15:34:01 ringo Exp $
 */



#define VCE_MAJOR_VERSION    0
#define VCE_MINOR_VERSION    4
#define VCE_RELEASE_VERSION  135

/*
  General info
 */
#define CE_URL "http://www.ce-lab.net/"
#define CE_COPYRIGHT "Virtual Community Engine Copyright by CommunityEngine Inc. 2000,2001,2002,2003\nSee http://www.ce-lab.net/"

   
#if defined linux || defined WIN32 || defined __APPLE__ || defined sun
/* Macros for intel machines */
#define GET_32BIT_INT(p)    ntohl( *((int*)(p)) )                     /* doc */
#define SET_32BIT_INT(p,i)    ((*((int*)(p))) = htonl((int)(i)))     /* doc */
#define GET_16BIT_INT(p)    ntohs( *((short*)(p)))         /* doc */
#define SET_16BIT_INT(p,i)    ((*((short*)(p))) = htons((short)(i)))  /* doc */
#define GET_8BIT_INT(p)    (*((char*)(p))) /* doc */
#define SET_8BIT_INT(p,i)  ((*((char*)(p)))=(char)(i)) /* doc */
#define GET_32BIT_NATIVEINT(p) ( *((int*)(p)) ) /* doc */
#define SET_32BIT_NATIVEINT(p,i) ((*((int*)(p))) = (int)(i)) /* doc */
#define GET_16BIT_NATIVEINT(p) ( *((short*)(p))) /* doc */
#define SET_16BIT_NATIVEINT(p,i) ((*((short*)(p))) = (short)(i)) /* doc */
#define GET_8BIT_NATIVEINT(p)  ( *((char*)(p))) /* doc */
#define SET_8BIT_NATIVEINT(p,i)  ((*((char*)(p))) = (char)(i)) /* doc */
#endif

#if defined CW_PW_PS2 || defined SCE_LIBNET_PS2
/* Funcs for MIPS machines */
int GET_32BIT_INT( char *p ); /* doc */
int SET_32BIT_INT( char *p, int i ); /* doc */
short GET_16BIT_INT( char *p ); /* doc */
short SET_16BIT_INT( char *p, short i ); /* doc */
#define GET_8BIT_INT(p)    (*((char*)(p))) /* doc */
#define SET_8BIT_INT(p,i)  ((*((char*)(p)))=(char)(i)) /* doc */
int GET_32BIT_NATIVEINT( char *p ); /* (osdep.c) doc */
int SET_32BIT_NATIVEINT( char *p, int i ); /* (osdep.c) doc */
short GET_16BIT_NATIVEINT( char *p ); /* (osdep.c) doc */
short SET_16BIT_NATIVEINT( char *p, short i ); /* (osdep.c) doc */
#define GET_8BIT_NATIVEINT(p)  ( *((char*)(p))) /* doc */
#define SET_8BIT_NATIVEINT(p,i)  ((*((char*)(p))) = (char)(i)) /* doc */
#endif

#ifndef MIN
#define MIN(x,y)   ( (x) < (y) ? (x) : (y) )   /* doc */
#endif
#ifndef MAX
#define MAX(x,y)   ( (x) > (y) ? (x) : (y) )   /* doc */
#endif
#define ELEMENTOF(x) ( sizeof(x) / sizeof((x)[0]) )  /* doc */



/* Block cipher algorithms */
typedef enum {
    VCE_BLOCK_CIPHER_NONE = 0,
    VCE_BLOCK_CIPHER_BLOWFISH = 1, /* from 4 to 56 bytes key length */
    VCE_BLOCK_CIPHER_RIJNDAEL = 2, /* 16,24,32 bytes keylen/blklen*/
    VCE_BLOCK_CIPHER_TWOFISH = 4,  /* 16,24,32 bytes keylen, 16bytes blklen */

    /* asymmetric encryption options, which should be appended by '|'  */
    VCE_BLOCK_CIPHER_ONLY_RECV = 8192,  
    VCE_BLOCK_CIPHER_ONLY_SEND = 16384
} VCE_BLOCK_CIPHER;

/* These constants are used by TCPcontext's IP source address filters. */
enum {
    FILTER_POLICY_DENY = 0,
    FILTER_POLICY_ACCEPT = 1,
    FILTER_POLICY_LOG = 2
};

#include <stdarg.h>

#if defined(linux) || defined(__APPLE__) || defined(sun) /* linux or MacOS X or Solaris*/
#include <time.h> /* for time_t */
#include <sys/time.h>  /* for timeval */
#include <sys/socket.h>
#include <netinet/in.h>
typedef long long VCEI64;
#endif

#ifdef CW_PW_PS2
#define __PS2__ 1 /* required for PlanetWeb TCP/IP stack headers */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
typedef long VCEI64; /* long is 64bit in Emotion Engine */
#endif


#ifdef SCE_LIBNET_PS2
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <libnet.h>
#include <libinsck.h>
/* Force have timeval */
struct timeval {
    unsigned int tv_sec;
    unsigned int tv_usec;
};
typedef long VCEI64; /* long is 64bit in Emotion Engine */
#endif


#ifdef WIN32
#include <time.h> /* for time_t */
#include <winsock2.h> /* for ntohs, ntohl, htons, htonl */
typedef __int64 VCEI64; /* GCC's long long is not available */
#endif



#ifdef __cplusplus    /* When the user is Using C++,use C-linkage by this */
extern "C"
{
#endif

#ifdef _USRDLL /* Defined under VisualStudio DLL project environment */
#define EXPORT _declspec (dllexport)
#else
#define EXPORT /* Don't have to use any special prefix under gcc */
#endif

/* init.c */
typedef struct _vce_limit_t
{
    int max_tcpcontext;
    int max_taskgroup;
    int max_conn;
	int max_search;
    int max_array;
    int sendbuf_work_size, recvbuf_work_size;
} vce_limit_t;

EXPORT void vce_limit_clear( vce_limit_t *t ); 
EXPORT int vce_initialize( void ); /* doc norb */
EXPORT int vce_initialize_limited( vce_limit_t *lim ); /* doc norb */
EXPORT int vce_finalize( void ); /* doc norb */
EXPORT void vce_init_limit_t( vce_limit_t *t ); /* doc norb */
		
/* uselog.c */
typedef struct _vce_stat_t
{
	time_t boot_time;
	VCEI64 recv_byte, send_byte; /* total vce send/recv */
    
	/* system call count */
	VCEI64 recv_syscall , recv_syscall_e;  
	VCEI64 send_syscall , send_syscall_e;
	VCEI64 accept_syscall , accept_syscall_e;
	VCEI64 connect_syscall , connect_syscall_e;

	VCEI64 conn_free , conn_alloc , conn_close , conn_write;
	VCEI64 keyex;
	VCEI64 tcpcontext_create , tcpcontext_cleanup;
	VCEI64 tcpcontext_connect;
	VCEI64 conn_timeout;
	VCEI64 log_count;  /* errout log count */
} vce_stat_t;


EXPORT int vce_errout_config( char *dirname,
                              unsigned int rotate_size ); /* doc rb*/
EXPORT void vce_set_verbose_mode( int m ); /* doc rb*/
EXPORT void vce_get_version( int *ma, int *mi, int *rel ); /* doc rb*/
EXPORT void vce_set_read_write_log_mode( int m ); /* doc rb*/
EXPORT void vce_errout( char *format, ... ); /* doc rb */
EXPORT void vce_verrout( char *format, va_list arg );
EXPORT void vce_errout_v( char *format, ... ); /* doc rb */
EXPORT void vce_errout0( char *s ); /* doc rb*/
EXPORT void vce_errout0_v( char *s ); /* doc rb*/
EXPORT void vce_get_stat( vce_stat_t *v ); /* doc */
EXPORT int vce_get_netstat_string( char *buf, int maxlen ); /* doc */


/* a macro for getting error string */
#define STRERR  (vce_strerror( vce_get_last_error()))  /* doc rb*/

/* BIN protocol max record length (1Gbytes) */
#define BIN16DATA_MAX ( 256 * 256  )
#define BIN32DATA_MAX ( 1024 * 1024 * 1024 )


/* Types */
#if defined __GNUC__
#if defined(linux) || defined(sun)
#pragma pack(1)
#endif
#elif defined(WIN32)
#pragma pack(push,1) /* Win32 VC only */
#elif defined(CW_PW_PS2)
#pragma pack(1)
#endif

typedef struct  /* circ_t will be passed to application by value. */
{
	void *p;  /* pointer to virtual circuit */
	unsigned int serial;
} circ_t;
typedef struct 
{
	void *p;     /* pointer to connection */
	unsigned int serial;
} conn_t;


typedef void * tcpcontext_t;
typedef void * taskcontext_t;
typedef void * taskgroup_t;
typedef void * switchcontext_t;


/* mshare.c */
typedef enum
{
    PRIO_IDLE = -1,
    PRIO_HIGHEST = 0,
    PRIO_0 = 0,
    PRIO_1 = 1,
    PRIO_2 = 2,
    PRIO_3 = 3,
    PRIO_LOWEST = 3
} LOCK_PRIO;
struct memshare_status
{
	unsigned int lock_request_count; /* increment when receiving lock_request*/
	unsigned int lock_release_count; /* increment when receiving lock_release*/
	unsigned int lock_timeout_count;
	unsigned int request_timeout_count;
	unsigned int uptime_sec; /* now - boottime  in second */
};

typedef void * memshare_cli_t;
typedef void * memshare_serv_t;


EXPORT memshare_cli_t * vce_mshare_client_init(
	char *buffer, unsigned int buffer_size,
	int (*sender)( char *data, unsigned int len ),
	void (*notify)( char *p, unsigned int ofs,
					unsigned int len , int timeo)); /* doc */
EXPORT void vce_mshare_client_cleanup( memshare_cli_t *mt ); /* doc */
EXPORT int vce_mshare_client_recv( memshare_cli_t *mtwrap,
								   char *buffer, unsigned int len ); /* doc */
EXPORT int vce_mshare_client_request_lock( memshare_cli_t *mtwrap,
										   unsigned int ofs, unsigned int size,
										   LOCK_PRIO reqprio ); /* doc */
EXPORT int vce_mshare_client_release_lock( memshare_cli_t *mtwrap,
										   unsigned int ofs,
										   unsigned int size ); /* doc */
EXPORT memshare_serv_t * vce_mshare_server_init(
	char *buffer,	unsigned int buffer_size,
	unsigned int lock_timeout_sec, unsigned int request_timeout_sec,
	unsigned int queue_len,
	int (*sender)( void * id, char *data, unsigned int len)); /* doc */
EXPORT void vce_mshare_server_cleanup( memshare_serv_t *mt ); /* doc */
EXPORT int vce_mshare_server_recv( memshare_serv_t *mtwrap,
								   void *id, char *buffer, int len ); /* doc */
EXPORT int vce_mshare_write_snapshot( memshare_serv_t *mtwrap,
									  char *fn,char *tmpfn ); /* doc */
EXPORT void vce_mshare_set_no_more_lock( memshare_serv_t *mtwrap ); /* doc */
EXPORT void vce_mshare_server_get_status( memshare_serv_t *mtwrap ,
										  struct memshare_status *st );/*doc*/


/* sw.c */
typedef enum
{
	SW_ACCEPTER_POLICY_TOP = 0,
	SW_ACCEPTER_POLICY_RANDOM = 1,
	SW_ACCEPTER_POLICY_SEQUENTIAL = 2
} SW_ACCEPTER_POLICY;
typedef enum
{
	SWENT_STATE_OFFLINE = 0,
	SWENT_STATE_CONNECTING = 1,
	SWENT_STATE_ONLINE = 2,
	SWENT_STATE_TIMEOUT = 3
} SWENT_STATE;
typedef struct _switchent_t
{
    char nb_ip[16]; /* network byte order IP address */
    int nb_ip_len;     /* length of IP address (4 if ipv4, 16 if ipv6) */
    unsigned short port;
	conn_t c;
	SWENT_STATE state;
	time_t time_till_offline; 
} switchent_t;

EXPORT switchcontext_t vce_switchcontext_create(
	switchent_t *st, int num,
	tcpcontext_t fectx, tcpcontext_t svctx,
	int rjbuf_size,
	SW_ACCEPTER_POLICY accepter_policy );

EXPORT int vce_switchtable_read_from_file( char *, switchent_t *,int *);/*doc*/
EXPORT void vce_switchtable_show( switchent_t *sw, int num );/*doc*/
EXPORT int vce_circ_switch( circ_t ct, int server_id, char *, int ); /* doc*/
typedef enum
{
	SWITCH_RESULT_OK = 0,
	SWITCH_RESULT_NOTFOUND = 1,
	SWITCH_RESULT_OFFLINE = 2,
	SWITCH_RESULT_BAD_STATEBUF = 3,
	SWITCH_RESULT_CIRC_FULL = 4,
    SWITCH_RESULT_CALLBACK_FAIL = 5
} SWITCH_RESULT;

EXPORT int vce_tcpcontext_set_circ_switchresultwatcher(
	tcpcontext_t tp, int (*w)(circ_t,int,int,char*,int)); /* doc */
EXPORT int vce_tcpcontext_set_circ_switchwatcher(
	tcpcontext_t tp, int (*w)(circ_t,int,char*,int)); /* doc */


/* vec.c */
EXPORT int vce_circ_write( circ_t cir, char *data, int datalen ); /* doc rb*/
EXPORT void vce_circ_close( circ_t cir ); /* doc rb*/
EXPORT int vce_circ_get_index( tcpcontext_t t, circ_t c ); /* doc */
EXPORT circ_t vce_circ_get_struct_from_index( tcpcontext_t t,int i);/*doc*/
EXPORT void * vce_circ_get_state_buffer( circ_t ct, int *len); /* doc */
EXPORT int vce_circ_is_valid( circ_t ct ); /* doc rb */
EXPORT int vce_circ_writable( circ_t ct ); /* doc rb */
EXPORT int vce_circ_is_equal( circ_t c1, circ_t c2 ); /* doc rb */
EXPORT int vce_protocol_parser_vec0( conn_t ct ); /* doc */
EXPORT int vce_protocol_closewatcher_vec0( conn_t ct ); /* doc */
EXPORT int vce_protocol_acceptwatcher_vec0( conn_t ct ); /* doc */
EXPORT int vce_vecprotocol_parser_bin16( circ_t cir ); /* doc rb*/
EXPORT int vce_vecprotocol_parser_bin32( circ_t cir ); /* doc rb */
EXPORT int vce_vecprotocol_parser_through( circ_t cir );/*doc rb*/
EXPORT int vce_vecprotocol_parser_text( circ_t cir ); /* doc rb*/
EXPORT int vce_vecprotocol_unparser_bin16( circ_t ct, char *,int); /* doc rb */
EXPORT int vce_vecprotocol_unparser_text( circ_t ct, char *d, int l);/*doc rb*/
EXPORT int vce_vecprotocol_unparser_through(circ_t ct,char *d,int l);/*doc rb*/
EXPORT int vce_vecprotocol_unparser_bin32( circ_t ct,char *d,int l);/*doc rb*/
EXPORT int vce_protocol_parser_vecmain_client( conn_t ct ); /* doc */
EXPORT int vce_vec_setup_linkage( tcpcontext_t tp, conn_t cp ); /* doc */
EXPORT int vce_protocol_parser_vecmain_server( conn_t ct ); /* doc */
EXPORT int vce_vec_init_mainserv( tcpcontext_t tp, int max, int rbs);/*doc*/
EXPORT void vce_circ_get_remote_ipv4_addr( circ_t ct, unsigned int *a,
										   unsigned short *p ); /* doc rb*/
EXPORT void vce_circ_get_remote_ipv6_addr( circ_t ct, char *a,
										   unsigned short *p ); /* doc */
EXPORT char *vce_circ_get_remote_ipv4_addr_string( circ_t ct, char *s,
												   int max ); /* doc */
EXPORT char *vce_circ_get_remote_ipv6_addr_string( circ_t ct, char *s,
													int max ); /* doc */
EXPORT char *vce_circ_get_remote_addr_string( circ_t ct, char *s,
											   int max ); /* doc */

EXPORT tcpcontext_t vce_circ_get_tcpcontext( circ_t ct ); /* doc */
EXPORT conn_t vce_circ_get_parent_conn( circ_t c);

EXPORT int vce_vecprotocol_acceptwatcher_noop( circ_t c ); /* doc rb*/
EXPORT int vce_vecprotocol_acceptwatcher_deny( circ_t c ); /* doc rb*/
EXPORT int vce_vecprotocol_closewatcher_noop( circ_t c ); /* doc rb*/
EXPORT void vce_circ_clear( circ_t *c ); /* doc */
EXPORT void vce_circ_show_stat( tcpcontext_t tp ); /* nodoc, debug */
EXPORT void vce_circ_set_timeout( circ_t ct, int sec ); /* doc */
EXPORT int vce_circ_test_sanity( tcpcontext_t tt, circ_t ct, int *type );

/* task.c */
EXPORT taskgroup_t vce_taskgroup_create( VCEI64 granu_us,
										 int num,
										 int worksize, int per_loop,
										 char *name ); /* doc */
EXPORT void vce_taskgroup_cleanup( taskgroup_t t ); /* doc */
void vce_taskgroup_set_scan_log_mode( taskgroup_t tp, int v ); /* doc */
EXPORT taskcontext_t vce_taskcontext_create(
	taskgroup_t tp, VCEI64 interval_us,
	int (*callback)( taskcontext_t tp, char *work, int len ),
	char *wk, int len  );  /* doc */
EXPORT void vce_taskcontext_cleanup( taskcontext_t tp );  /* doc */
EXPORT int vce_taskcontext_get_id( taskcontext_t tp ); /* doc */
typedef struct _taskgroup_stat_t
{
	VCEI64 total_taskgroup_check_count;
	VCEI64 total_taskcontext_check_count;
	VCEI64 total_taskcontext_call_count;
	int inuse_num;
	int max_num;
} taskgroup_stat_t;
EXPORT void vce_taskgroup_get_stat( taskgroup_t tp, taskgroup_stat_t *);/*doc*/
EXPORT void *vce_taskcontext_get_workmem( taskcontext_t tp );/*doc*/
EXPORT VCEI64 vce_taskcontext_get_elapsed_time( taskcontext_t t ); /*doc*/

/* conn.c */
EXPORT int vce_conn_write( conn_t ct, char *b, size_t len ); /* doc rb*/
EXPORT int vce_conn_writable( conn_t ct  ); /* doc rb*/
EXPORT int vce_conn_writed( conn_t ct ); /* doc rb*/
EXPORT void vce_conn_close( conn_t ct ); /* doc rb*/
EXPORT void vce_conn_show_stat( void ); /* doc rb*/
EXPORT void vce_conn_get_remote_ipv4_addr( conn_t ct, unsigned int *a,
										   unsigned short *p  ); /* doc rb*/
EXPORT void vce_conn_get_remote_ipv6_addr( conn_t ct, char *a,
                                           unsigned short *p ); /* doc */
EXPORT void vce_conn_get_local_ipv4_addr( conn_t ct, unsigned int *a,
                                          unsigned short *p  ); /* doc rb*/
EXPORT void vce_conn_get_local_ipv6_addr( conn_t ct, char *a,
                                          unsigned short *p ); /* doc */
EXPORT void vce_conn_get_local_ipv6_addr( conn_t ct, char *a,
										  unsigned short *p ); /* doc */
EXPORT char * vce_conn_get_remote_ipv4_addr_string( conn_t ct,
													char *s,int max);/*doc*/
EXPORT char * vce_conn_get_local_ipv4_addr_string( conn_t ct,
												   char *s,int max);/*doc*/
EXPORT char * vce_conn_get_remote_ipv6_addr_string( conn_t ct,
                                                    char *,int);/* doc */
EXPORT char * vce_conn_get_local_ipv6_addr_string( conn_t ct,
												   char *,int);/* doc */
EXPORT char * vce_conn_get_remote_addr_string( conn_t ct,
                                                  char *, int); /* doc */
EXPORT char * vce_conn_get_local_addr_string( conn_t ct,
											  char *, int); /* doc */
EXPORT int vce_conn_get_index( conn_t c ); /* doc */
EXPORT conn_t vce_conn_get_struct_from_index( int index ); /* doc */
EXPORT void * vce_conn_get_state_buffer( conn_t c, int *len ); /* doc */
EXPORT tcpcontext_t vce_conn_get_tcpcontext( conn_t ct ); /* doc */
EXPORT int vce_conn_is_valid( conn_t c ); /* doc rb */
EXPORT int vce_conn_is_equal( conn_t c1, conn_t c2 ); /* doc rb */	
EXPORT void vce_conn_get_buffer( conn_t ct, char **rb, int *rblen,
								 char **wb, int *wblen ); /* doc */
EXPORT int vce_conn_shrink_readbuf( conn_t ct, int len ); /* doc */
EXPORT void vce_conn_clear( conn_t *ct ); /* doc */
EXPORT void vce_conn_close_tcpcontext_all( tcpcontext_t t );/*doc*/
typedef struct _conn_stat_t
{
	VCEI64 recv_byte, send_byte, conn_write;
	VCEI64 recv_syscall, send_syscall;
} conn_stat_t;
EXPORT void vce_conn_get_stat( conn_t ct, conn_stat_t *st ); /* doc */
EXPORT void vce_conn_set_timeout( conn_t ct, int sec ); /* doc */
EXPORT void vce_conn_set_parser( conn_t ct, int (*p)(conn_t),
                                 int (*pcb)(conn_t,char*,int));/* doc */

/* tcp.c */
EXPORT tcpcontext_t vce_tcpcontext_create( int is_server,
                                           char *bindaddr,
                                           unsigned short port, int maxcon ,
                                           size_t rblen , size_t wblen,
                                           int timeout ,
                                           VCE_BLOCK_CIPHER encrypt,
                                           int shkey_len,
                                           int nonblock_connect,
                                           int nodelay,
                                           size_t statebuf_size );/* doc rb*/

EXPORT void vce_tcpcontext_set_circ_finalizer( tcpcontext_t t,
											   void (*f)(circ_t));/*doc norb*/
EXPORT void vce_tcpcontext_set_conn_finalizer( tcpcontext_t t,
											   void (*f)(conn_t));/*doc norb*/

EXPORT void vce_tcpcontext_cleanup( tcpcontext_t t ); /* doc rb*/
EXPORT conn_t vce_tcpcontext_connect( tcpcontext_t t,
									  char *h , unsigned short p ); /* doc rb*/
EXPORT int vce_tcpcontext_get_all_connection( tcpcontext_t t ,
											  conn_t *a, int max); /* doc rb*/
EXPORT int vce_tcpcontext_is_circ_mode( tcpcontext_t tp ); /* doc rb*/

EXPORT void vce_tcpcontext_set_conn_parser( tcpcontext_t tp,
											int (*parser)(conn_t),
											int (*c)(conn_t,
													 char*,int));/*doc rb*/
EXPORT void vce_tcpcontext_set_circ_parser( tcpcontext_t tp,
											int (*parser)(circ_t),
											int (*c)(circ_t,
													 char*,int));/*doc rb*/
EXPORT void vce_tcpcontext_set_conn_acceptwatcher( tcpcontext_t tp,
											int (*aw)(conn_t) );/* doc rb*/
EXPORT void vce_tcpcontext_set_conn_hiwater_acceptwatcher(
    tcpcontext_t tp, int water, int (*aw)(conn_t, int)); /* doc */
EXPORT void vce_tcpcontext_set_circ_acceptwatcher( tcpcontext_t tp,
											int (*aw)(circ_t) );/* doc rb*/
EXPORT void vce_tcpcontext_set_circ_hiwater_acceptwatcher(
    tcpcontext_t tp, int water, int (*aw)(circ_t, int)); /* doc */
typedef enum
{
    CLOSE_REASON_NONE = -1,   /* used inside VCE */
    CLOSE_REASON_UNKNOWN = 0, /* reason unknown */
    CLOSE_REASON_REMOTE, /* socket closed by remote peer */
    CLOSE_REASON_APPLICATION,   /* socket closed by application */
    CLOSE_REASON_TIMEOUT, /* close by timeout */
    CLOSE_REASON_DECODER, /* protocol decoder fail */
    CLOSE_REASON_ENCODER, /* protocol encoder fail */
    CLOSE_REASON_PARSER, /* parser returns minus */
    CLOSE_REASON_INTERNAL, /* VCE internal error */
    CLOSE_REASON_FULL, /* (sw) buffer full */
    CLOSE_REASON_ROUTE, /* change vpr route */
} CLOSE_REASON;
EXPORT void vce_tcpcontext_set_conn_closewatcher( tcpcontext_t tp,
                      int (*cw)(conn_t, CLOSE_REASON r )); /* doc rb*/
EXPORT void vce_tcpcontext_set_circ_closewatcher( tcpcontext_t tp,
                      int (*c)(circ_t, CLOSE_REASON r ));/* doc rb*/
EXPORT int vce_tcpcontext_add_conn_ipv4_filter( tcpcontext_t tp,
												char *net,char *mask,
												int policy ); /* doc rb*/
EXPORT int vce_tcpcontext_delete_conn_ipv4_filter( tcpcontext_t tp,
												   char *,char *);/* doc rb*/
EXPORT int vce_tcpcontext_add_circ_ipv4_filter( tcpcontext_t tp,
												char *net,char *mask,
												int policy ); /* doc rb */
EXPORT int vce_tcpcontext_delete_circ_ipv4_filter( tcpcontext_t tp,
												   char *,char *); /* doc rb*/
EXPORT void vce_tcpcontext_clear_conn_ipv4_filter( tcpcontext_t tp );/* doc */
EXPORT void vce_tcpcontext_clear_circ_ipv4_filter( tcpcontext_t tp );/* doc */
EXPORT int vce_tcpcontext_use_filter_file( tcpcontext_t tp, char *p);/* doc */
EXPORT int vce_tcpcontext_set_conn_safety_read( tcpcontext_t tp, int p);/*doc*/
EXPORT void vce_tcpcontext_set_read_delay( tcpcontext_t tp,
										   int min_delay_ms, int max_delay_ms,
										   int distrib ); /* doc */
EXPORT void vce_tcpcontext_enable_accept( tcpcontext_t tp, int flag );/*doc*/
typedef struct _tcpcontext_stat_t
{
    int current_conn, current_circ;
	VCEI64 recv_byte, send_byte;
	VCEI64 recv_syscall, recv_syscall_e;
	VCEI64 send_syscall, send_syscall_e;
	VCEI64 accept_syscall, accept_syscall_e;
	VCEI64 connect_syscall, connect_syscall_e;
	VCEI64 conn_write, conn_timeout, tcpcontext_connect;
} tcpcontext_stat_t;
EXPORT void vce_tcpcontext_get_stat( tcpcontext_t tp,
									 tcpcontext_stat_t *s ); /*doc*/
EXPORT int vce_tcpcontext_circ_iterate( tcpcontext_t tt,
										int (*f)(circ_t c) ); /* doc */
EXPORT int vce_tcpcontext_conn_iterate( tcpcontext_t tt,
										int (*f)(conn_t c)); /* doc */
EXPORT void vce_tcpcontext_set_conn_call_parser_per_heartbeat(
    tcpcontext_t tt, int v ); /* doc */
EXPORT void vce_tcpcontext_timeout_control( tcpcontext_t tp, int recv_reset,
                                            int send_reset ); /* doc */
EXPORT void vce_tcpcontext_set_equal_io_splitrate(tcpcontext_t tt,
                                                  int v);/*doc*/
EXPORT int vce_tcpcontext_count_connections( tcpcontext_t t, int *conn,
                                             int *circ ); /* doc */

/* protocol.c */
EXPORT int vce_protocol_parser_bin16( conn_t ct ); /* doc rb*/
EXPORT int vce_protocol_parser_bin32( conn_t ct ); /* doc rb */
EXPORT int vce_protocol_parser_through( conn_t ct ); /* doc rb*/
EXPORT int vce_protocol_parser_text( conn_t ct ); /* doc rb*/
EXPORT int vce_protocol_unparser_bin16( conn_t ct, char*,int); /* doc rb*/
EXPORT int vce_protocol_unparser_bin32( conn_t ct, char*,int); /* doc rb*/
EXPORT int vce_protocol_unparser_through( conn_t ct, char *,int); /* doc rb*/
EXPORT int vce_protocol_unparser_text( conn_t ct, char *d, int l );/*doc rb*/
EXPORT int vce_protocol_acceptwatcher_deny( conn_t c ); /* doc rb*/
EXPORT int vce_protocol_acceptwatcher_noop( conn_t c ); /* doc rb*/
EXPORT int vce_protocol_closewatcher_noop( conn_t c ); /* doc rb*/

/* pcallback.c */
EXPORT int vce_protocol_pcallback_echo_bin16(conn_t c,char *d,int l);/*doc rb*/
EXPORT int vce_protocol_pcallback_echo_text(conn_t ct,char *d,int l);/*doc rb*/
EXPORT int vce_protocol_pcallback_noop(conn_t ct ,char *d, int l );/*doc rb*/
EXPORT int vce_protocol_pcallback_dump(conn_t ct, char *d, int l );/*doc rb*/
EXPORT int vce_vecprotocol_pcallback_echo_bin16(circ_t ct,
												char *data,int len);/*doc rb*/
EXPORT int vce_vecprotocol_pcallback_echo_text(circ_t c,char*,int);/*doc rb*/


/* util.c */
EXPORT void *MALLOC( size_t size ); /*doc no-rb*/
EXPORT void FREE( void *p ); /*doc no-rb*/
EXPORT void vce_dump_buffer( char *buf, int len ); /*doc rb*/
EXPORT int vce_read_simple_config_file( char *filename, 
										int(*c)(char*,int,
												char*,char*));/*doc no-rb*/
EXPORT int vce_read_plain_text_file( char *fn,
									 int (*cb) (char*,int,char*)); /* doc */
EXPORT int vce_sha1_file_checksum(char *path , char *out , int len );/*doc rb*/
EXPORT void vce_sha1_buffer_checksum(char *buf,int len,
									 char *out,int outlen);/*doc rb*/
EXPORT void vce_make_binary_array_from_hex(char *out,
										   int outlen,char *hex); /*doc no-rb*/
EXPORT void vce_make_binary_array_from_line(char *line,int linelen,char *out,
											int*outlen,int*ul);/*doc no-rb*/
EXPORT int vce_make_hex_from_binary_array( char *out, int outlen,
                                           char *buf,int len,
                                           int folding); /* doc no-rb */
EXPORT int vce_make_hex_text_from_buffer( char *buf , int len ,
										  char *out , int outlen ); /*doc rb*/
EXPORT int vce_extract_data_from_hex_text( char *buf, int len,
										   char *out, int outlen );/*doc rb*/

EXPORT VCEI64 vce_get_current_utime( void ); /* doc */
EXPORT VCEI64 vce_utime_diff( struct timeval *b, struct timeval *a); /* doc */

/* cipher.c */
EXPORT int vce_encode_buffer_by_blowfish(char *b,int l,
										 char *k,int kl);/*doc rb*/
EXPORT int vce_decode_buffer_by_blowfish(char*b,int l,char*k,int kl);/*doc rb*/
EXPORT int vce_encode_buffer_by_rijndael(char *b,int l,
                                         char *k, int kl);/*doc*/
EXPORT int vce_decode_buffer_by_rijndael(char*b,int l,
                                         char*k,int kl);/*doc rb*/
EXPORT void vce_replace_malloc_funcs( void* (*a)(size_t),
                                      void* (*r)(void*,size_t),
                                      void (*f)(void*)); /* doc */

/* osdep.c */
EXPORT int vce_get_ipv4_addr_by_name( unsigned int *a, char *s );/*doc rb*/
EXPORT void vce_snprintf( char *out, int len, char *format, ... ); /* doc */
EXPORT int vce_scandir( char *path, char *suf, int (*cb)(char *p)); /* doc */


/* mem.c */
#define ARRAY_SCAN(ind,x)   for( (x) = vce_get_array_object_top(ind);x;(x)=vce_get_array_object_next(ind,x)) /* doc */

#define ARRAY_SCAN_INDEX( ind, x, i) for( i = 0,(x) = vce_get_array_object_top(ind);x;(x)=vce_get_array_object_next(ind,x),i++)

#define ARRAY_SCAN_TYPE(ind,x,type)   for( (x) = (type)vce_get_array_object_top(ind);x;(x)=(type)vce_get_array_object_next(ind,x)) /* doc */

#define ARRAY_SCAN_TYPE_INDEX(ind,x,type, i)   for( i = 0, (x) = (type)vce_get_array_object_top(ind);x;(x)=(type)vce_get_array_object_next(ind,x),i++) /* doc */

EXPORT int vce_init_array ( size_t size , int num , char *nm ); /* doc */
EXPORT int vce_end_array( int index ); /* doc */
EXPORT void *vce_alloc_array_object( int index ); /* doc */
EXPORT int vce_free_array_object( int index , void *p ); /* doc */
EXPORT void *vce_get_array_object_top( int index ); /* doc */
EXPORT void * vce_get_array_object_next( int index , void *p ); /* doc */
EXPORT void * vce_get_array_object_last( int index ); /* doc */
EXPORT int vce_get_array_usenum( int index ); /* doc */
EXPORT int vce_get_array_object_index( int index , void *p ); /* doc */
EXPORT void * vce_get_array_object_pointer_by_index( int i, int obji ); /*doc*/
EXPORT void * vce_get_array_object_pointer_by_index_only_used(
	int index , int objindex ); /* doc */
EXPORT int vce_show_array_usage( int index , int show_all ); /* doc */
EXPORT void vce_end_all_array( void ); /* doc */
EXPORT int vce_if_included_in_array( int index, void *p ); /* doc */
EXPORT void vce_free_array_object_all( int index ); /* doc */
EXPORT int vce_get_array_member_size( int index ); /* doc */
EXPORT int vce_get_array_maxnum( int index ); /* doc */
EXPORT int vce_array_object_inuse( int index, void *p ); /* doc */

/* search.c */
EXPORT int vce_init_int_search( int hsz , int ent_num , char *sname );/*doc*/
EXPORT int vce_update_int_search( int index , int key , void *pointer );/*doc*/
EXPORT void * vce_get_int_search( int index , int key ); /*doc*/
EXPORT int vce_delete_int_search( int index , int key ); /*doc*/
EXPORT int vce_init_int2_search( int hsz , int ent_num , char *sname );/*doc*/
EXPORT int vce_update_int2_search( int ind, int k0 , int k1, void *p); /*doc */
EXPORT void * vce_get_int2_search( int ind , int k0, int k1  ); /*doc*/
EXPORT int vce_delete_int2_search( int ind , int k0, int k1  ); /*doc */
EXPORT int vce_iterate_int2_search( int index, int (*callback)(void*)); /*doc*/
EXPORT int vce_init_string_search( int hsz , int entnum , char *sname );/*doc*/
EXPORT int vce_update_string_search( int ind , char *k , void *p );/*doc*/
EXPORT void * vce_get_string_search( int index , char *key );/*doc*/
EXPORT int vce_delete_string_search( int index , char *key );/*doc*/
EXPORT void vce_cleanup_all_search_engine( void );/*doc*/
EXPORT int vce_cleanup_search_engine(int index ); /* doc */
EXPORT int vce_delete_vstring_search( int index, char *key );
EXPORT int vce_update_vstring_search( int index, char *key, void *pointer );
EXPORT void * vce_get_vstring_search( int index, char *key );
EXPORT int vce_init_vstring_search( int hashtbl_size, int ent_num,
                                    int key_len, char *sname );
EXPORT int vce_get_search_usenum( int index ); /* doc */


/* loop.c */
EXPORT void vce_set_mainloop_callback( int ( *f )(void) ); /* doc rb*/
EXPORT int vce_mainloop( void ); /* doc rb*/
EXPORT void vce_heartbeat( void ); /* doc rb*/
EXPORT void vce_set_heartbeat_wait_flag( int flag ); /* doc */
EXPORT void vce_set_internal_timer( time_t t, unsigned int microsec ); /*doc*/
EXPORT void vce_set_socket_library_ok( int flag ); /* doc */

/* text.c */
EXPORT int vce_httptext_divide_header_and_content( char *in , int inlen,
												   char **head, int *headlen,
												   char **b, int *);/*doc rb*/
EXPORT int vce_text_cmpnocase( char *s1, char *s2, int n ); /*doc rb*/
EXPORT int vce_httptext_get_header_info( char *h, int hlen ,
										 char *i , char *o, int);/*doc rb*/
EXPORT char * vce_text_split_index( char *src, char c, int index ); /*doc rb*/
EXPORT int vce_copy_cstring_upto( char *d, char *s, char c,int );/*doc rb*/
EXPORT int vce_copy_cstring( char *dest , int l , char *src ); /* doc no-rb*/
EXPORT int vce_text_split_index_get_int(char *s,char,int,int *v);/* doc rb */
EXPORT int vce_text_split_index_get_string( char *src, char c,
											int ind,char *out,int );/*doc rb*/
EXPORT int vce_strcmptail( char *s1, char *s2 ); /* doc rb*/
EXPORT int vce_text_hashpjw( char *s ); /* doc rb*/
EXPORT char * vce_makecstr( char *, int, char *, int ); /*doc no-rb*/
EXPORT char * vce_chop_string_destroy( char *s ); /* doc no-rb */
EXPORT int vce_strlenmax( char *s, int max ); /* doc */
EXPORT int vce_strncat( char *out, int outlen, char *append );/*doc*/
EXPORT void vce_strncpy( char *to, int to_len, const char *from, int from_len ); /* doc */
EXPORT int vce_escape_string( char *to, int tolen, char *from, int frl);/*doc*/
EXPORT int vce_descape_string( char *to, int tolen, char *from ); /*doc*/

/* bin.c */
typedef enum
{
    SINGLE_CHAR,
    SINGLE_SHORT,
    SINGLE_INT,
    CHARARRAY_8,
    CHARARRAY_16,
    CHARARRAY_32, 
    SHORTARRAY_8,
    SHORTARRAY_16,
    SHORTARRAY_32, 
    INTARRAY_8,
    INTARRAY_16,
    INTARRAY_32,
	STRARRAY_8,
	STRARRAY_16
} BINDATATYPE;

struct bin_info
{
	BINDATATYPE t;
	unsigned int maxu; /* Not used when packing */
    unsigned int n;
    void *p;
};

EXPORT int vce_binary_unpack( struct bin_info *bi,int,char *,int);/*doc rb*/
EXPORT int vce_binary_pack( struct bin_info *bi,int,char *,int );/*doc rb*/

int vce_binary_push_nb_i1( char *out, int ofs, int maxlen, char c );/*doc*/
int vce_binary_push_nb_i2( char *out, int ofs, int maxlen, short s );/*doc*/
int vce_binary_push_nb_i4( char *out, int ofs, int maxlen, int i );/*doc*/
int vce_binary_push_nb_ia1( char *out, int ofs, int maxlen,
							char *ia, int num );/*doc*/
int vce_binary_push_nb_ia2( char *out, int ofs, int maxlen,
							short *ia, int num );/*doc*/
int vce_binary_push_nb_ia4( char *out, int ofs, int maxlen,
							int *ia, int num );/*doc*/
int vce_binary_push_nb_stra( char *out, int ofs, int maxlen,
							char **stra, int num, int eachmax );/*doc*/
int vce_binary_pop_nb_i1( char *in, int len, int ofs, char *out);/*doc*/
int vce_binary_pop_nb_i2( char *in, int len, int ofs, short *out );/*doc*/
int vce_binary_pop_nb_i4( char *in, int len, int ofs, int *out );/*doc*/
int vce_binary_pop_nb_ia1( char *src, int len, int ofs,
						   char *out, int *outlen );/*doc*/
int vce_binary_pop_nb_ia2( char *src, int len, int ofs,
						   short *out, int *outlen );/*doc*/
int vce_binary_pop_nb_ia4( char *src, int len, int ofs,
						   int *out, int *outlen );/*doc*/
int vce_binary_pop_nb_stra( char *src, int len, int ofs,
							char **out, int *strnum, int eachmax );/*doc*/

int vce_set_variable_length_specifier( char *out, int outmax,
									   unsigned int len_upper,
									   unsigned int len_lower );/*nodoc*/
int vce_get_variable_length_specifier( char *src, int srcmax,
									   unsigned int *len_upper,
									   unsigned int *len_lower );/*nodoc*/

/* dh.c */
void vce_diffie_hellman( char *genstr , char *modstr , char *expstr,
						 char *out, int outlen ); /* nodoc */

/* istore.c */
typedef struct _istore
{
	char dname[256];
} istore;
EXPORT int vce_istore_init( istore *is, char *dn, int ndc  ); /* doc rb*/
EXPORT int vce_istore_save( istore *is, unsigned int k, char *, int);/*doc rb*/
EXPORT int vce_istore_load( istore *is, unsigned int k, char *, int);/*doc rb*/

/* ecode.c */
EXPORT char * vce_strerror( int ecode ); /* doc rb*/
EXPORT int vce_get_last_error( void ); /* doc rb*/

/* monitor.c */
EXPORT int vce_monitor_server_enable( int listen_port,
                                      int maxcon, char *key ); /* doc */
EXPORT void vce_monitor_server_disable(void); /* doc */
EXPORT int vce_monitor_server_add_command( char* name, int argc, int (*callback)( conn_t _c, int argc, char **argv ), char *desc );
EXPORT int vce_monitor_server_reply( conn_t _c, int result, char *output );
EXPORT int vce_monitor_command_netstat( conn_t _c, int argc, char **argv );
EXPORT int vce_monitor_command_ping( conn_t _c, int argc, char **argv );
EXPORT int vce_monitor_command_get_pid( conn_t _c, int argc, char **argv );
EXPORT int vce_monitor_command_get_port( conn_t _c, int argc, char **argv );
EXPORT int vce_monitor_command_get_conn( conn_t _c, int argc, char **argv );
EXPORT int vce_monitor_command_suicide( conn_t _c, int argc, char **argv );
EXPORT int vce_monitor_command_prof( conn_t _c, int argc, char **argv );
EXPORT int vce_monitor_server_add_standard_commands();
EXPORT int vce_monitor_client_enable( int maxmonitor ); /* doc */
EXPORT void vce_monitor_client_set_replywatcher(
    void (*w)(int id,int serial, int result, char*s,int l)); /* doc */
EXPORT void vce_monitor_client_disable( void ); /* doc */
typedef struct _monitor_ent_t
{
    int id;
    unsigned short port;
    char host[128];
    char pass[128];
    int login;
    conn_t co;
} monitor_ent_t;
EXPORT void vce_monitor_ent_init( monitor_ent_t *m,
                                  int id, char *host,
                                  unsigned short port,
                                  char *pass ); /* doc */
EXPORT int vce_monitor_client_add( monitor_ent_t *m ); /* doc */
EXPORT void vce_monitor_client_delete( int id ); /* doc */
EXPORT int vce_monitor_client_connect( int id ); /* doc */
EXPORT int vce_monitor_client_disconnect( int id ); /* doc */
EXPORT int vce_monitor_client_connected( int id ); /* doc */
EXPORT int vce_monitor_client_call( int id, int serial, char *s ); /* doc */
EXPORT int vce_monitor_client_callable( int id ); /* doc */

/* profiler.c */
#if defined(linux) || defined(sun)
EXPORT int vce_profiler_init( int max_tags );
EXPORT void vce_profiler_cleanup( void );
EXPORT int vce_profiler_in( char *tag );
EXPORT int vce_profiler_out( char *tag );
EXPORT void vce_profiler_save_stats( char *path );
#endif

/* analyzer.c */
#define VCE_ANALYZER_PROTOCOL_MAX     2048
typedef struct _analyzer_t {
    int protocol_max;                      // プロトコル番号の最大値(256以上かどうかだけが重要)
    struct {
        int count;                         // 送信した回数
        int size;                          // 送信したサイズ
        int fail_count;                    // 失敗した回数
        int fail_size;                     // 失敗したサイズ
    } protocol[VCE_ANALYZER_PROTOCOL_MAX];

    int total_count;
    int total_size;
    int total_fail_count;
    int total_fail_size;
} analyzer_t;

void vce_analyzer_init( analyzer_t *a, int protocol_max );
int vce_analyzer_analyze( analyzer_t *a, char *data, int data_len, int is_fail );
void vce_analyzer_clear( analyzer_t *a );
int vce_analyzer_get_state_string( analyzer_t *a, char *buf, int buf_len );

/* vcelog.c */
/* callback function type same as printf() */
typedef int (*printf_func_t)( const char *format, ... );

/* callback function type returning a time string */
typedef void (*gettime_func_t)( char *buf, int len );

void vce_log_init( printf_func_t printf_func );
int vce_log_add_type( int type, char *tag );
void vce_log_set_type_mask( unsigned int type );
void vce_log_set_type_mask_by_name( char *name, int on_off );
void vce_log_set_gettime_func( gettime_func_t gettime );
void vce_log_get_types( char *buf, int buf_len );

void vce_log_gettime_normal( char *buf, int len );
void vce_log_gettime_long( char *buf, int len );
void vce_log_gettime_short( char *buf, int len );

void vce_log_vprint( int type, char *id, char *char_name, char *format, va_list va );
void vce_log_print( int type, char *id, char *char_name, char *format, ... );

void vce_log_parse( char *line, int line_len,
                    char *date, int date_len,
                    char *type, int type_len,
                    char *id, int id_len,
                    char *name, int name_len,
                    char *text, int text_len );

int vce_log_add_monitor_command();

#define LOGWRITER_BUF_SIZE      8192 // a buffer size of logwriter
typedef struct _logwriter_t {
    int id;                          // Log ID
    int state;
    char addr[32];                   // remote address
    int port;                        // remote port
    conn_t conn;                     
    VCEI64 last_connect;             
    int fail_count;                  
    char fail_buf[LOGWRITER_BUF_SIZE];
    int fail_buf_pos;                
    int set_id_sended;               // if set_log_id send
} logwriter_t;

int vce_logwriter_init( int num );
logwriter_t *vce_logwriter_connect( char *addr, int port, int id );
int vce_logwriter_poll();

int vce_logwriter_printf( logwriter_t *w, const char *format, ... );
int vce_logwriter_vprintf( logwriter_t *w, const char *format, va_list va );


#if defined __GNUC__ 
#if defined(linux) || defined(sun)
#pragma pack()
#endif
#elif defined(WIN32)
#pragma pack(pop) /* Win32 VC only */
#elif defined(CW_PW_PS2)
#pragma pack(0)  /* CodeWarrior only */
#endif

#ifdef __cplusplus
}
#endif



#define VCE_NOERROR (0)      /* No error */
#define VCE_EGENERIC (-1)    /* Generic error */
#define VCE_EFULL  (-2)      /* Array/Mem/Buffer full. */
#define VCE_EBUG   (-3)      /* It's a bug. */
#define VCE_ERANGE (-5)      /* Value is out of range */
#define VCE_EINVAL (-6)      /* Bad value */
#define VCE_EMEM   (-7)      /* Memory shortage */
#define VCE_ENOENT (-8)      /* No such entry in [search|array] */
#define VCE_EIO    (-9)      /* I/O error ( read/write error ) */
#define VCE_ESOCKET (-10)    /* Socket error  */
#define VCE_EBIND (-11)      /* bind error ( already server is running? ) */
#define VCE_EIPADDR ( -12)   /* IP address error */
#define VCE_ELISTEN (-13)    /* Listen error */
#define VCE_ECONFIG (-14)    /* Bad configuration */
#define VCE_EHOSTNAME (-15)  /* Bad host name */
#define VCE_EFCNTL (-16)     /* Cannot set socket flags */
#define VCE_ECONNECT (-17)   /* Cannot connect  */
#define VCE_EBUFSMALL (-18)  /* Buffer is too small */
#define VCE_EPROTO    (-19)  /* Protocol error */
#define VCE_ENOTREADY (-20)  /* (Connection) is not ready */
#define VCE_ENEGO    (-21)  /* Encryption negotiation error */
#define VCE_EPROTO_INVAL (-22) /* invalid argument (protocol) */
#define VCE_EPROTO_ODD (-23)   /* Odd number of input bytes (protocol)*/
#define VCE_EPROTO_TOOLONG (-24) /* Data too long (protocol)*/
#define VCE_ENOCALLBACK (-25)  /* Callback function is not set */
#define VCE_ESUPPORT  (-26)    /* Not supported currently */
#define VCE_ECIRCSERIAL (-27)  /* Virtual circuit serial number error */
#define VCE_ECONNSERIAL (-28)  /* TCP connection serial number error */
#define VCE_EVECNOTREADY (-29) /* Vectorizer is not initialized */
#define VCE_ENOCIRCPARENT (-30) /* No parent connection for a circuit */
#define VCE_EALIGNMENT (-32)    /* The object has long alignment */
#define VCE_EFORMAT (-33)       /* Bad string format */
#define VCE_ESEARCH (-34)       /* Search engine error */
#define VCE_ECONTENTLEN (-35)   /* Bad content length in protocol */
#define VCE_ECONNINIT (-36)     /* Connection table is not initialized */
#define VCE_EBADKEYLEN (-37 )   /* Bad key length */
#define VCE_EACCEPT (-38 )      /* Accept error */
#define VCE_ESETSOCKOPT (-39)   /* setsockopt() error */
#define VCE_EACCESSFILE (-40)   /* cannot access the file */
#define VCE_EWINSOCKINIT (-41)  /* Winsock initialization error */
#define VCE_ELOCKINFO (-42)     /* Bad lock-information */
#define VCE_EWBFULL (-44)       /* write buffer is full */
#define VCE_ELONGINPUT (-45)    /* input buffer is too long */
#define VCE_ETOOSHORT (-46)     /* input buffer is too short */
#define VCE_EPROTO_BADLEN (-47) /* bad content length (protocol) */
#define VCE_EPROTO_RESULT (-48) /* error is indicated by result code (protocol) */
#define VCE_EPROTO_SERVERINVAL (-49) /* invalid argument by server (protocol)*/
#define VCE_EPROTO_BADCOMMAND (-50) /* bad command (protocol)*/
#define VCE_EFOPEN_W (-51)      /* cannot open file to write */
#define VCE_ERENAME (-52)       /* rename(2) fail */
#define VCE_ENOMORELOCK (-53)   /* no more lock (mshare) */
#define VCE_ENOTFOUND (-54)     /* not found */
#define VCE_ELENTOOLONG (-55)   /* len is too long */
#define VCE_EDATASHORT (-56)    /* actual data is too short */
#define VCE_EOPENDIR (-57)      /* cannot open directory for some reason */
#define VCE_EFSEEK (-58)        /* cannot seek file */
#define VCE_EFWRITE (-59)       /* cannot write data to file */
#define VCE_EFREAD (-60)        /* cannot read data from file */
#define VCE_EMKDIR (-61)        /* cannot create a directory */
#define VCE_EALREADY (-62)      /* operation is already done */
#define VCE_EFINALIZE (-63)     /* cannot finalize VCE */
#define VCE_ECLOSED (-64)       /* already closed */
#define VCE_EGENCOMMAND (-65)   /* protocol generator command error */
#define VCE_ENOCRYPTO (-66)     /* no such crypto engine */
#define VCE_ETOOLONGNAME (-67)  /* specified name is too long */
#define VCE_ESIZE (-68)         /* invalid size */
#define VCE_ETASK (-69)         /* cannot create internal task */
#define VCE_ETASKG (-70)        /* cannot create internal task group */
#define VCE_ESWNOTREADY (-71)   /* switchcontext is not ready */
#define VCE_ESERVERDOWN (-72)   /* server is down */
#define VCE_ESWTRANSIENT (-73)  /* now the circ is in transient state */
#define VCE_EODDBUFSIZE (-74)   /* odd buffer size (cipher) */
#define VCE_EODDKEYSIZE (-75)   /* odd key size (cipher) */
#define VCE_EKEYEX (-76)        /* key exchange negotiaion error */
#define VCE_EMORESENDBUF (-77)  /* need more send buffer */
#define VCE_EINIT (-78)         /* vce not initialized */
#define VCE_EARRAY (-79)         /* needs more internal array */
#define VCE_ESETROUTE (-80)     /* vpr cannot set route */

/* Fatal error messages */
#define FATAL_WINSOCK "Fatal:1000 cannot initialize winsock\n"
#define FATAL_FLQ_ARRAY_S "Fatal:1001 cannot initialize fileloader queue ( not used after 0.3.17 ): %s\n"
#define FATAL_FL_TCPCTX_S "Fatal:1002 cannot initialize fileloader tcpc ( not used after 0.3.17 ): %s\n"
#define FATAL_ARRAY_SCARCE "Fatal:1003 array table scarcity (init_array)\n"
#define FATAL_ARRAY_FMALLOC_D "Fatal:1010 MALLOC fail (index=%d)\n"
#define FATAL_ARRAY_FMALLOC1_D "Fatal:1011 MALLOC fail (index=%d)\n"
#define FATAL_ARRAY_FMALLOC2_D "Fatal:1012 MALLOC fail (index=%d)\n"
#define FATAL_ARRAY_FULL_S "Fatal:1020 array full in %s\n"
#define FATAL_ODD_BUFFER_SIZE_D "Fatal:1050 odd buffer size: %d\n"
#define FATAL_MSH_ODD256_BUFFER_SIZE "Fatal:1080 please set buffer with length of n*256 bytes.\n" 
#define FATAL_SINT_FMALLOC1 "Fatal:1090 int-search: MALLOC fail\n"
#define FATAL_SINT_INITA_D "Fatal:1091 int-search: init_array fail (index=%d)\n"
#define FATAL_SINT_UPDATE1_D "Fatal:1092 int-search: entry full! index=%d\n"
#define FATAL_SINT_UPDATE2_D "Fatal:1093 int-search: entry full! index=%d\n"
#define FATAL_SSTR_FMALLOC1 "Fatal:1094 str-search: MALLOC fail\n"
#define FATAL_SSTR_INITA_D "Fatal:1095 str-search: init_array fail (index=%d)\n"
#define FATAL_SSTR_UPDATE1_D "Fatal:1097 str-search entry full! index=%d\n"
#define FATAL_SSTR_UPDATE2_D "Fatal:1098 str-search entry full! index=%d\n"
#define FATAL_SINT2_FMALLOC1 "Fatal:1100 int2-search: MALLOC fail\n"
#define FATAL_SINT2_INITA_D "Fatal:1101 int2-search: init_array fail index=%d\n"
#define FATAL_SINT2_UPDATE1_D "Fatal:1102 int2-search: entry full! index=%d\n"
#define FATAL_SINT2_UPDATE2_D "Fatal:1103 int2-search: entry full! index=%d\n"
#define FATAL_TCP_NO_MORE_CON "Fatal:1200 tcp: no more connections are availabe\n"
#define FATAL_TCP_BAD_KEYLEN_D "Fatal:1201 tcp: bad secret key length: %d\n"
#define FATAL_TCP_BAD_ENCRYPT_TYPE_D "Fatal:1202 tcp: bad encryption type specified:%d\n"
#define FATAL_TCP_NEED_PARSER "Fatal:1203 need a parser when creating tcpcontext\n"
#define FATAL_TCP_SOCKET_S_D_S "Fatal:1204 socket() fail addr:%s port:%d os_err:%s\n"
#define FATAL_TCP_GETHOSTBYNAME_S "Fatal:1205 gethostbyname() error: %s\n"
#define FATAL_TCP_GETHOSTBYNAME2_S "Fatal:1206 gethostbyname() error: %s\n"
#define FATAL_TCP_BIND_S_S_D "Fatal:1207 bind() error. os_err:%s addr:%s port:%d\n"
#define FATAL_TCP_LISTEN_S_S_D "Fatal:1208 listen() error. os_err:%s addr:%s port:%d\n"
#define FATAL_TCP_SETSOCKOPT_S "Fatal:1209 setsockopt() fail. os_err:%s\n"
#define FATAL_TCP_GETHOSTBYNAME3_S "Fatal:1210 gethostbyname() error: %s\n"
#define FATAL_TCP_FCNTL_S "Fatal:1212 fcntl fail: %s\n"
#define FATAL_TCP_SETSOCKOPT2_S "Fatal:1213 setsockopt() fail: %s\n"
#define FATAL_TCP_STARTKEYEX_D "Fatal:1214 keyex_start_client fail on fd:%d\n"
#define FATAL_USELOG_WRITE1_S "Fatal:1230 cannot write data in %s\n"
#define FATAL_USELOG_WRITE2_S "Fatal:1231 cannot write data in %s\n"
#define FATAL_VEC_INIT_SEARCH "Fatal:1250 cannot init vpaddr search engine\n"
#define FATAL_VEC_INITMAIN_MALLOC "Fatal:1251 cannot MALLOC master rb.\n"
#define FATAL_VEC_ALLOC_CIRC_ARRAY "Fatal:1252 cannot allocate circuit array\n"
#define FATAL_VEC_INIT_INT2SEARCH_S "Fatal:1253 cannot initialize int2 search engine:%s\n"
#define FATAL_TCP_CLOSECTX_NULL "Fatal:1256 pointer to tcpcontext is null\n"
#define FATAL_TCP_GETHOSTBYNAMEWIN_S "Fatal:1260 win32 fatal: gethostbyname() error:%s\n"
#define FATAL_TCP_STATEBUF_INIT "Fatal:1265 Memory shortage when creating tcpcontext state buffer\n"
#define FATAL_VEC_REALLOCATE_STATEBUF_S "Fatal:1268 Cannot reallocate vec's state buffer array. (%s)"
#define FATAL_VCE_SCANDIR_NOTSUPPORTED "Fatal:1270 vce_scandir is not supported on this operating system\n"
#define FATAL_TCP_SETSOCKOPT_REUSE_S  "Fatal:1277 setsockopt(SO_REUSEADDR) fail:%s\n"
#define FATAL_INVALID_CONN_IPV4_FILTER_S_S_D "Fatal:1280 cannot add ipv4 filter for conn: serious bug? net:%s mask:%s flags:%d\n"
#define FATAL_INVALID_CIRC_IPV4_FILTER_S_S_D "Fatal:1281 cannot add ipv4 filter for circ: serious bug? net:%s mask:%s flags:%d\n"

/* Warning messages */
#define WARN_FREE_NULL_S "Warning:4000 freeing null pointer in %s\n"
#define WARN_FREE_OUTR_S_P_P "Warning:4001 freeing an out-of-bound element in %s address:%p master-address:%p\n"
#define WARN_FREE_ALIGN_S_P_P_U "Warning:4002 freeing an oddly-aligned element in %s address:%p master-address:%p sizeof:%u\n"
#define WARN_FREE_OUTR2_S_P_P_U "Warning:4003 freeing an out-of-bound element in %s address:%p master-address:%p sizeof:%u\n"
#define WARN_MSH_SERVER_PROTO1 "Warning:4010 server packet has bad length\n"
#define WARN_MSH_RQ_FULL "Warning:4011 server: request queue is full\n"
#define WARN_MSH_SV_SHUTDOWN "Warning:4012 server: being shutting down\n"
#define WARN_MSH_SV_INVAL "Warning:4013 server: invalid argument\n"
#define WARN_MSH_UNKNOWN_D "Warning:4014 unknown server error: %d\n"
#define WARN_MSH_REQ_TIMEO_U_U "Warning:4015 request timeout for ofs:%u len:%u\n"
#define WARN_MSH_UNKNOWN_REPLY_X "Warning:4016 unknown reply from server:0x%x\n"
#define WARN_MSH_BADSEND_D_D "Warning:4017 sender() returns %d not %d\n"
#define WARN_MSH_BADSEND2_D_D "Warning:4018 sender() returns %d not %d\n"
#define WARN_MSH_TIMEO_LENGTH "Warning:4040 request_timeout_sec should be at least 2 times longer than lock_timeout_sec\n"
#define WARN_NO_MORE_LOCK_P "Warning:4050 mshare: no more lock on %p\n"
#define WARN_MSH_BADSEND3_S "Warning:4051 mshare: sender: %s\n"
#define WARN_MSH_BADSEND4_S "Warning:4052 mshare: sender: %s\n"
#define WARN_MSH_LOCKNOOWN_D_D_D_U_U "Warning:4054 mshare: doesn't own lock. ol[%d].used=%d ol[%d].id=%u id=%u\n"
#define WARN_MSH_BADSEND5_S "Warning:4055 mshare: sender: %s\n"
#define WARN_MSH_BADSEND6_S "Warning:4056 mshare: sender: %s\n"
#define WARN_MSH_BADSEND7_S "Warning:4057 mshare: sender: %s\n"
#define WARN_MSH_NOTIFY_REQTIMEO_U_U "Warning:4060 notify request timeout to client: ofs:%u len:%u\n"
#define WARN_MSH_TRY_LOCK_ERROR_S "Warning:4061 try_lock error: %s\n"
#define WARN_MSH_LOCK_EXPIRE_D "Warning:4062 lock expired ol[%d]\n"
#define WARN_BFENC_NOSPACE_D "Warning:4080 bfenc: no space left on %d's ewb\n"
#define WARN_TCP_ONLY_CON_D "Warning:4100 tcp: only %d connections are usable\n"
#define WARN_TCP_ACCEPT_S_D "Warning:4101 tcp: accept() error. os_err:%s port:%d\n"
#define WARN_TCP_MAXCON_D_D "Warning:4102 tcp: conn_alloc fail no more accept. max:%d port:%d\n"
#define WARN_TCP_ACCEPTW_D "Warning:4103 tcp: acceptwatcher fail fd:%d\n"
#define WARN_TCP_SELECT_MINUS_D_S "Warning:4104 select() returns %d: %s\n"
#define WARN_TCP_TIMEOUT_CON_D_D "Warning:4105 connection timeout. fd:%d sec:%d\n"
#define WARN_TCP_HIWATER_ACCEPTW_D "Warning: 4106 tcp: hiwater acceptwatcher fail fd:%d\n"
#define WARN_VEC_BUFFER_FULL "Warning:4200 vecp buffer full. closing client connection.\n"
#define WARN_VEC_BUFFER_FULL2 "Warning:4201 vecp buffer full. closing client connection.\n"
#define WARN_VEC_FSEARCH_D "Warning:4202 vecp cannot update search engine on fd:%d\n"
#define WARN_VEC_BUFFER_FULL3 "Warning:4203 vecp buffer full. closing client connection.\n"
#define WARN_VEC_BAD_PROTO_MAGIC "Warning:4204 vecp: bad protocol magic\n"
#define WARN_VEC_BAD_VERSION "Warning:4205 vecp: bad version number\n"
#define WARN_VEC_BAD_CLEN_D "Warning:4206 vecp: bad content length:%d\n"
#define WARN_VEC_BAD_PROTO_MAGIC2 "Warning:4207 vecsv: bad protocol magic on vp connection\n"
#define WARN_VEC_BAD_VERSION2 "Warning:4208 vecsv: bad version number on vp connection\n"
#define WARN_VEC_BAD_CLEN2_D "Warning:4209 vecsv: bad content length:%d\n"
#define WARN_VEC_EWRITE "Warning:4210 vecsv: cannot write data into vp connection\n"
#define WARN_VEC_WRITE_CIRC_D_D_D_D_D "Warning:4211 vecsv: cannot write to circuit buffer. ret:%d content:%d start:%d end:%d len:%d\n"
#define WARN_REFREE_S_P "Warning:4214 re-freeing an element in %s address:%p\n"
#define WARN_TCP_SELECT_WINERROR_D "Warning:4215 winsock select() returns SOCKET_ERROR, and WSAGetLastError:%d\n"
#define WARN_KEYGEN_SHA_KEY_D "Warning:4220 SHA-1 keygen output buffer is smaller than 40byte. It usually needs 40 byte. len:%d\n"
#define WARN_RETURNING_EWBFULL_D_D "Warning:4223 bin16: unparser cannot write full record. try:%d actual:%d\n"
#define WARN_TCP_MAX_CONN_IN_USE_D "Warning:4225 vce_tcpcontext_create: reached max connection num %d\n"
#define WARN_CONN_DOES_NOT_HAVE_PARSER "Warning:4300 vce_conn_io: conn does not have parser function. use vce_tcpcontext_set_conn_parser\n"
#define WARN_CIRC_DOES_NOT_HAVE_PARSER "Warning:4301 vce_protocol_parser_vecmain_server: circ does not have parser function. use vce_tcpcontext_set_circ_parser\n"
#define WARN_TCP_CONN_FILTER_DENY_S "Warning:4310 tcpcontext_try_accept: IP source address filter denies this address: %s\n"
#define WARN_TCP_CIRC_FILTER_DENY_S "Warning:4311 vectorizer server: IP source address filter denies this address: %s\n"
#define WARN_TCP_NO_MORE_ACCEPT "Warning:4312 tcpcontext_try_accept: no more accept\n"


/* Internal bug detection messages */
#define BUG_FREESTACK_OVERFLOW_S "Bug:7000 blank-list stack overflow: %s\n"
#define BUG_SHOW_ARRAY_USAGE_S_P "Bug:7002 use flag is 0! in %s address:%p\n"
#define BUG_USENUM_NOMATCH_S_D_D "Bug:7003 usenum doesn't match in %s stack:%d link_inuse:%d\n"
#define BUG_TCP_NOBUF "Bug:7010 tcp: nobuf?\n"
#define BUG_TCP_NOMEM "Bug:7011 tcp: nomem?\n"
#define BUG_VEC_DELETE_SEARCH "Bug:7100 vecp: cannot delete search entry\n"
#define BUG_VEC_NOTFOUND_CLI "Bug:7101 vecp: cannot find client connection\n"
#define BUG_VEC_NOTFOUND_CLI2 "Bug:7102 vecp: cannot find client connection\n"
#define BUG_VEC_DELETE_SEARCH2 "Bug:7103 vecsv: cannot delete search entry\n"
#define BUG_VEC_NOTFOUND_CLI3 "Bug:7104 vecsv: cannot find client connection\n"
#define BUG_VEC_DELETE_SEARCH3 "Bug:7105 vecsv: cannot delete client circuit from search engine.\n"
#define BUG_VEC_DELETE_SEARCH_WHEN_CLOSE "Bug:7106 vecsv: vce_circ_close fail\n"   
#define BUG_TCP_STATEBUF_RUNSHORT "Bug:7109 state buffer is full.(accept)\n"
#define BUG_TCP_CONNECT_STATEBUF_RUNSHORT "Bug:7112 connect: statebuf shortage\n"

/* Notify messages */
#define NOTE_INIT_MSHARE_U_U "Note:10000 mshare init: unitnum:%u size:%u\n"
#define NOTE_NOOP_PROTOCOL_D_D "Note:10001 NOOP PROTOCOL: %d bytes from conn_t. serial:%d\n"
#define NOTE_DUMP_PROTOCOL_D_D "Note:10002 DUMP PROTOCOL: %d bytes from conn_t. serial:%d\n"
#define NOTE_BFDEC_INIT_D "Note:10010 init blowfish decoder on fd:%d\n"
#define NOTE_BFENC_INIT_D "Note:10011 init blowfish encoder on fd:%d\n"
#define NOTE_RIJDEC_INIT_D "Note:10012 init rijindael decoder on fd:%d\n"
#define NOTE_RIJENC_INIT_D "Note:10013 init rijndael encoder on fd:%d\n"
#define NOTE_SINT_OK_S_D_D "Note:10100 init int-search OK name:%s htsize:%d entnum:%d\n"
#define NOTE_SSTR_OK_S_D_D "Note:10101 init str-search OK name:%s htsize:%d entnum:%d\n"
#define NOTE_SINT2_OK_S_D_D "Note:10102 init int2-search OK name:%s htsize:%d entnum:%d\n"
#define NOTE_SE_CLEAN_S "Note:10103 cleaned up search engine %s\n"
#define NOTE_TCP_NEW_CONNECTION "Note:10200 new connection fd:%d I:%d sbuflen:%d\n"
#define NOTE_TCP_NONBLOCK_CON "Note:10201 non blocking connect().\n"
#define NOTE_TCP_CONNECT_OK_D "Note:10202 connect ok on %d\n"
#define NOTE_UTIL_MALLOC_U_P "Note:10220 malloc(%u) returns %p\n"
#define NOTE_UTIL_FREE_P "Note:10221 free(%p)\n"
#define NOTE_VEC_REGISTER_D "Note:10300 vec REGISTERED remoteport:%d\n"
#define NOTE_VEC_UNREGISTER_X_D "Note:10301 vec UNREGISTERED remoteaddr:%x:%p\n"
#define NOTE_VEC_CLOSE_FROM_SERVER "Note:10302 vec CLOSE command from server\n"
#define NOTE_VEC_ACCEPT_OK_X_D "Note:10303 vec ACCEPT OK on %x:%d\n"
#define NOTE_VEC_CLOSE_OK_X_D "Note:10304 vec CLOSE OK on %x:%d\n"
#define NOTE_INIT_ARRAY_OK_D_P_U_D_S "Note:10310 init_array OK index:%d bufmaster:%p size:%u num:%d name:%s\n"
#define NOTE_CONN_EOF_D_S "Note:10320 conn_t detected EOF on fd %d addr:%s\n"
#define NOTE_CONN_READERROR_D_S "Note:10321 conn_t detected read error on fd %d addr:%s\n"
#define NOTE_CONN_WRITEERROR_D_S_S "Note:10322 conn_t detected write error on fd %d (%s) addr:%s\n"
#define NOTE_CONN_NEGO_ERROR_D_D "Note:10323 conn_t detected encryption negotiation error on fd %d error code:%d\n"
#define NOTE_CONN_DECODING_ERROR_D_D "Note:10324 conn_t detected decoding error on fd %d error code: %d\n"
#define NOTE_CONN_ENCODING_ERROR_D_D "Note:10325 conn_t detected encoding error on fd %d error code: %d\n"
#define NOTE_CONN_PROTOCOL_ERROR_D_D "Note:10326 conn_t detected protocol error on fd %d error code: %d\n"
#define NOTE_KEYEX_BAD_CLIPUBKEYLEN_D "Note:10330 key exchange: bad client public key length: %d\n"
#define NOTE_KEYEX_NEGO_DONE_D "Note:10331 key exchange: encryption negotiation is done on fd %d\n"
#define NOTE_KEYEX_NEGO_DONE2_D "Note:10332 key exchange: encryption negotiation is done on fd %d\n"
#define NOTE_KEYEX_BAD_MAJOR_VERSION_D "Note:10333 key exchange: bad encryption version %d\n"
#define NOTE_KEYEX_BAD_SECRET_KEY_LENGTH_D "Note:10334 key exchange: bad secret key length: %d\n"
#define NOTE_VEC_CLOSE_OK2_X_D "Note:10338 vce_circ_close successful:%x %d\n"
#define NOTE_VEC_REALLOCATE_STATEBUF_D "Note:10340 vec: reallocated state buffer size:%d\n"
#define NOTE_TCP_STATEBUF_ALLOC "Note:10342 allocated state buffer for conn.\n"
#define NOTE_TCP_STATEBUF_INIT_OK "Note:10345 vce_tcpcontext_create could allocate state buffer.\n"
#define NOTE_TCP_FREED_STATEBUF "Note:10347 freed state buffer for conn\n"
#define NOTE_VEC_STATEBUF_ALLOC_OK "Note:10348 vec: statebuf allocate ok\n"
#define NOTE_VEC_FREED_STATEBUF "Note:10349: vec: freed state buffer for a circ\n"
#define NOTE_VCE_FINALIZED "Note:10400: vce: finalized\n"


#define STRDEF_EMESG_NOERROR        "No error"
#define STRDEF_EMESG_EGENERIC       "Generic error"
#define STRDEF_EMESG_EFULL          "Array/Mem/Buffer full"
#define STRDEF_EMESG_EBUG           "It can be a bug"
#define STRDEF_EMESG_ERANGE         "Value is out of range"
#define STRDEF_EMESG_EINVAL         "invalid argument"
#define STRDEF_EMESG_EMEM           "Memory shortage"
#define STRDEF_EMESG_ENOENT         "No such entry in [search|array]"
#define STRDEF_EMESG_EIO            "I/O error ( read/write error )"
#define STRDEF_EMESG_ESOCKET        "Socket error"
#define STRDEF_EMESG_EBIND          "bind error ( already server is running ?)"
#define STRDEF_EMESG_EIPADDR        "IP address error"
#define STRDEF_EMESG_ELISTEN        "Listen error"
#define STRDEF_EMESG_ECONFIG        "Bad configuration"
#define STRDEF_EMESG_EHOSTNAME      "Bad host name"
#define STRDEF_EMESG_EFCNTL         "Cannot set socket flags"
#define STRDEF_EMESG_ECONNECT       "Cannot connect"
#define STRDEF_EMESG_EBUFSMALL      "Buffer is too small"
#define STRDEF_EMESG_EPROTO         "protocol error"
#define STRDEF_EMESG_ENOTREADY      "(Connection) is not ready"
#define STRDEF_EMESG_ENEGO          "Encryption negotiation error"
#define STRDEF_EMESG_EPROTO_INVAL   "Protocol error ( invalid argument )"
#define STRDEF_EMESG_EPROTO_ODD     "Odd number of input bytes (protocol)"
#define STRDEF_EMESG_EPROTO_TOOLONG "Data too long (protocol)"
#define STRDEF_EMESG_ENOCALLBACK    "Callback function is not set"
#define STRDEF_EMESG_ESUPPORT       "Not supported currently"
#define STRDEF_EMESG_ECIRCSERIAL    "Virtual circuit serial number error"
#define STRDEF_EMESG_ECONNSERIAL    "TCP connection serial number error"
#define STRDEF_EMESG_EVECNOTREADY   "Vectorizer is not initialized"
#define STRDEF_EMESG_ENOCIRCPARENT  "No parent connection for a circuit"
#define STRDEF_EMESG_EALIGNMENT     "The object has long alignment"
#define STRDEF_EMESG_EFORMAT        "Bad string format"
#define STRDEF_EMESG_ESEARCH        "Search engine error"
#define STRDEF_EMESG_ECONTENTLEN    "Bad content length in protocol"
#define STRDEF_EMESG_ECONNINIT      "Connection table is not initialized"
#define STRDEF_EMESG_EBADKEYLEN     "Bad key length"
#define STRDEF_EMESG_EACCEPT        "Accept error"
#define STRDEF_EMESG_ESETSOCKOPT    "setsockopt() error"
#define STRDEF_EMESG_EACCESSFILE    "cannot access file ( generic )"
#define STRDEF_EMESG_EWINSOCKINIT   "winsock init fail"
#define STRDEF_EMESG_ELOCKINFO      "bad lock information (mshare)"
#define STRDEF_EMESG_EMSHAREINIT     "cannot initialize mshare module"
#define STRDEF_EMESG_EWBFULL        "write buffer is full."
#define STRDEF_EMESG_ELONGINPUT     "input data is too long"
#define STRDEF_EMESG_ETOOSHORT      "input data is too short"
#define STRDEF_EMESG_EPROTO_BADLEN  "bad content length (protocol)"
#define STRDEF_EMESG_EPROTO_RESULT  "error in result code (protocol)"
#define STRDEF_EMESG_EPROTO_SERVERINVAL "invalid argument by server(protocol)"
#define STRDEF_EMESG_EPROTO_BADCOMMAND "bad command (protocol)"
#define STRDEF_EMESG_EFOPEN_W       "cannot open file to write (fopen fail)"
#define STRDEF_EMESG_ERENAME        "cannot rename a file (rename fail)"
#define STRDEF_EMESG_ENOMORELOCK    "no more lock request (mshare)"
#define STRDEF_EMESG_ENOTFOUND      "not found (generic) "
#define STRDEF_EMESG_ELENTOOLONG    "len is too long"
#define STRDEF_EMESG_EDATASHORT     "actual data is too short"
#define STRDEF_EMESG_EOPENDIR       "cannot open directry"
#define STRDEF_EMESG_EFSEEK         "cannot seek file"
#define STRDEF_EMESG_EFWRITE        "cannot write data to file"
#define STRDEF_EMESG_EFREAD         "cannot read data from file"
#define STRDEF_EMESG_EMKDIR         "cannot create a directory"
#define STRDEF_EMESG_EALREADY       "operation is already done"
#define STRDEF_EMESG_EFINALIZE      "cannot finalize VCE"
#define STRDEF_EMESG_ECLOSED        "already closed"
#define STRDEF_EMESG_EGENCOMMAND    "protocol generator command error"
#define STRDEF_EMESG_ENOCRYPTO      "no such crypto engine"
#define STRDEF_EMESG_ETOOLONGNAME   "specified name is too long"
#define STRDEF_EMESG_ESIZE          "invalid size"
#define STRDEF_EMESG_ETASK          "cannot create internal taskcontext"
#define STRDEF_EMESG_ETASKG         "cannot create internal taskgroup"
#define STRDEF_EMESG_ESWNOTREADY    "switchcontext is not ready"
#define STRDEF_EMESG_ESERVERDOWN    "server is down"
#define STRDEF_EMESG_ESWTRANSIENT   "now the circ is in transient state"
#define STRDEF_EMESG_EODDBUFSIZE    "odd buffer size (cipher)"
#define STRDEF_EMESG_EODDKEYSIZE    "odd key size (cipher)"
#define STRDEF_EMESG_EKEYEX         "key exchange negotiation error"
#define STRDEF_EMESG_EMORESENDBUF   "need more send buffer"
#define STRDEF_EMESG_EINIT          "vce not inintialized"
#define STRDEF_EMESG_EARRAY         "need more internal array"
#define STRDEF_UNKNOWNERROR         "Unknown error code"



#endif /* VCE_H */
