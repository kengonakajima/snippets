#ifndef _COMMON_H_
#define _COMMON_H_




/* 一般化されている関数はここでプロトタイプする。 */

/* Init/Finish */
int p_initServerSocket( int http_port , int ftp_port );
void p_closeServerSocket( int s );

/* Log */
void p_printError( char *s );
void p_print( char *s );

/* Polling */
void p_preparePolling( int sockfd );
void p_doPolling( int sockfd );
void p_finishPolling( int mainsock  );
int p_getMinPollingCount( void );
int p_getMaxPollingCount( void );


/* Read / Write / Close flag*/
int p_socketRead( int s , char *buf , int maxlen );
int p_socketReadLine( int s , char *buf , int maxlen );
int p_socketReadHTTPHeader( int s , char *buf , int maxlen );
int p_socketWrite( int s , char *buf , int len );
void p_setCloseFlag( int s );
int p_isClosed( int s );

/* Connect */
int p_connectToHTTPServer( char *h , int port );

/* Protocol detect */
int p_isHTTP( int fd );
int p_isFTP( int fd );

/* 関連づけ */
void p_associateSocket( int c , int s );
int p_getAssociatedSocket( int so );

/* 状態操作 */
void p_startHTTPBody( int s );
int p_isHTTPHeaderState( int s );
int p_isClientSocket( int s );


#endif
