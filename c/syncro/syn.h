#ifndef _SYN_H_

#define _SYN_H_


int initSyncro( unsigned short port , int session_id , int my_id );
int addSyncroAddr( char *ipaddr , unsigned short port);
int negotiateSyncroHosts( int *total, int *now );
int closeSyncroAddr( char *ipaddr , unsigned short port );
void doSyncro( void );
void finishSyncro( void );
int addSyncroData(
 char *name , void *pointer , int count , char *type, int diff , int local );
void printSyncroLog( char *out , int maxlen );

/* win32 だったらいらん */

typedef int SOCKET;
typedef int BOOL;
#define FALSE 0
#define TRUE  1


#endif /* ifdef _SYN_H_ */
