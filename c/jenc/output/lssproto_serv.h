/* output by lsgen.perl 0.41 ( 1998 May)
 * made Sun Nov 15 21:12:56 1998
 * user ringo
 * host jap
 * file /home/ringo/test/jenc/output/lssproto_serv.h
 * util output/lssproto_util.c , output/lssproto_util.h
 * src  /home/ringo/test/jenc//home/ringo/ls2/ls2/doc/lssproto.html
 */
#ifndef _LSSPROTOSERV_H_
#define _LSSPROTOSERV_H_
#include "lssproto_util.h"

#ifdef MAXLSRPCARGS
#if ( MAXLSRPCARGS <= ( 6 + 1 )  )
#undef MAXLSRPCARGS
#define MAXLSRPCARGS ( 6 + 1 ) 
#endif
#else
#define MAXLSRPCARGS ( 6 + 1 ) 
#endif
void lssproto_W_recv( int fd,char* direction,int id ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 71 */
void lssproto_W_send( int fd,int id,int x,int y ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 107 */
void lssproto_AD_recv( int fd,int dir ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 138 */
void lssproto_MGD_recv( int fd,int dir,char* pattern ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 147 */
void lssproto_SKD_recv( int fd,int dir,int index ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 161 */
void lssproto_CM_recv( int fd,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 207 */
void lssproto_ID_recv( int fd,int dir,int itemindex ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 221 */
void lssproto_ST_recv( int fd,int titleindex ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 239 */
void lssproto_DT_recv( int fd,int titleindex ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 253 */
void lssproto_FT_recv( int fd,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 269 */
void lssproto_PI_recv( int fd,int dir ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 286 */
void lssproto_DI_recv( int fd,int itemindex ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 300 */
void lssproto_DG_recv( int fd,int amount ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 316 */
void lssproto_MI_recv( int fd,int fromindex,int toindex ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 330 */
void lssproto_SKUP_recv( int fd,int skillindex ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 357 */
void lssproto_MSG_recv( int fd,int index,char* message,int color ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 370 */
void lssproto_AB_recv( int fd ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 387 */
void lssproto_AB_send( int fd,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 393 */
void lssproto_DAB_recv( int fd,int index ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 411 */
void lssproto_AAB_recv( int fd ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 422 */
void lssproto_NAB_send( int fd,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 428 */
void lssproto_RAB_recv( int fd,int index ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 441 */
void lssproto_TK_recv( int fd,char* message,int color ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 455 */
void lssproto_T_send( int fd,int charindex,char* message,int color ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 470 */
void lssproto_M_recv( int fd,int fl,int x1,int y1,int x2,int y2 ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 491 */
void lssproto_M_send( int fd,int fl,int x1,int y1,int x2,int y2,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 506 */
void lssproto_C_send( int fd,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 536 */
void lssproto_CA_send( int fd,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 601 */
void lssproto_R_send( int fd,int result,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 633 */
void lssproto_S_recv( int fd,char* category ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 665 */
void lssproto_S_send( int fd,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 684 */
void lssproto_D_send( int fd,int category,int dx,int dy,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 855 */
void lssproto_ClientLogin_recv( int fd,char* cdkey,char* passwd ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 887 */
void lssproto_ClientLogin_send( int fd,char* result ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 902 */
void lssproto_CreateNewChar_recv( int fd,char* charname,int imgno,char* option ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 913 */
void lssproto_CreateNewChar_send( int fd,char* result,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 947 */
void lssproto_CharDelete_recv( int fd,char* charname ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 968 */
void lssproto_CharDelete_send( int fd,char* result,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 977 */
void lssproto_CharLogin_recv( int fd,char* charname ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 992 */
void lssproto_CharLogin_send( int fd,char* result,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 999 */
void lssproto_ChangePasswd_recv( int fd,char* oldpasswd,char* newpasswd ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 1012 */
void lssproto_ChangePasswd_send( int fd,char* result,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 1021 */
void lssproto_CharList_recv( int fd ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 1033 */
void lssproto_CharList_send( int fd,char* result,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 1036 */
void lssproto_CharLogout_recv( int fd ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 1056 */
void lssproto_CharLogout_send( int fd,char* result,char* data ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 1060 */
void lssproto_Echo_recv( int fd,char* test ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 1073 */
void lssproto_Echo_send( int fd,char* test ) ; /* /home/ringo/ls2/ls2/doc/lssproto.html line 1081 */
int lssproto_InitServer(int (*writefunc)(int,char*,int) ,	int worksiz );
void lssproto_CleanupServer( void );
void lssproto_ServerDispatchMessage( int fd, char *line );
#endif
/* end of the generated server header code */