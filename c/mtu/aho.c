#include <netinet/in.h>
#include <arpa/inet.h>
#include <ioctls.h>
#include <stdio.h>
#include <errno.h>

/*
  
  MTU�򥲥åȤ���
  
 */
int main()
{
    int s = socket(AF_INET,SOCK_DGRAM,0);
    int i;
    
    fprintf( stderr , "%d\n", ioctl( s , SIOCGIFMTU , &i ) );
    fprintf( stderr , "%s\n", strerror( errno ));
    close(s);
}
