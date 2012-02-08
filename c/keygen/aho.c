/*
  Keygen
  �Υƥ���

  ZBD ����Ѥ����ñ���������ͥ졼��

  
 */

#include "sha.h"
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <fcntlbits.h>


/*
  �������������롢��������������ʤ�Ρ�

  getpid , time , times, /dev/random �򤹤٤�SHA-1�ˤ��ϥå��夹�롣

  char *k : ���ϥ���
  int len : ����Ĺ��
  
 */
void
generate_private_key( char *k , int len )
{
    unsigned long pid = getpid();
    time_t now = time(NULL);
    SHA_INFO sha;

    sha_init( &sha );

    sha_update( &sha, (SHA_BYTE*) &now , sizeof( now ));
    sha_update( &sha, (SHA_BYTE*) &pid , sizeof( pid ));
    {
        clock_t ticks;
        struct tms tms;        
        ticks = times( &tms );
        sha_update( &sha, (SHA_BYTE*) &ticks , sizeof( ticks ));
        sha_update( &sha, (SHA_BYTE*) &tms, sizeof( tms ));
    }
    {
        /* /dev/random ���Ȥ���Ȥ��ϻȤ��� */
        int fd = open( "/dev/random" , O_RDONLY );
        char buffer[8];
        if( fd != -1 ){
            read( fd, buffer, 8 );
            sha_update( &sha, (SHA_BYTE*)buffer, 8 );
            close(fd);
        }
    }

    sha_final( &sha );

    snprintf( k , len , "%08lx%08lx%08lx%08lx%08lx",
              sha.digest[0],sha.digest[1],sha.digest[2],
              sha.digest[3],sha.digest[4] );
    
}

    
int
main( int argc, char **argv )
{
    char k[100];

    generate_private_key ( k , sizeof(k) );

    printf( "%s\n", k );

    
}
