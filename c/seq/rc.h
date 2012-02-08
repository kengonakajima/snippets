
int rc_open( unsigned long ip_net ,unsigned short port_net );
int rc_init( unsigned short port ,int maxslot );
int rc_accept( void );
int rc_write( int fd ,char *buf , int len );
int rc_read( int fd,  char *buf , int len  );
int rc_close( int fd );
int rc_proc(void);



double getUTimeDouble(void);
