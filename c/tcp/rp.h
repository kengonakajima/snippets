#ifndef _RP_H_

#define _RP_H_


int rp_init( unsigned short port , int maxcon);
int rp_open( unsigned long ip_net ,unsigned short port_net );
int rp_accept( unsigned long *ip_net , unsigned short *port_net );
int rp_send( int fd, char *buf , int len );
int rp_recv( int fd , char *buf , int len );
int rp_close( int fd );
int rp_proc( void);
int rp_isclosed( int fd );

int rp_recvable(int fd );
int rp_sendable( int fd );
int rp_acceptable(void );


double getUTimeDouble(void);

#endif /* _RP_H_ */

