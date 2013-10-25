#include <stdio.h>
#include <sys/socket.h> // PF_PACKET, SOCK_RAW
#include <net/ethernet.h> // ETH_P_ALL
#include <net/if.h> // struct ifreq
#include <sys/ioctl.h> // ioctl, SIOCGIFINDEX
#include <netpacket/packet.h> // struct sockaddr_ll
#include <net/if_arp.h>

#include <string.h> // memset
#include <errno.h>


int main( int argc, char *argv ) {
    int sockfd = socket( PF_PACKET, SOCK_RAW, htons( ETH_P_ALL ));   // need root
    if(sockfd<0) {
        perror("socket");
        return 1;
    }
    printf("sockfd:%d \n", sockfd );

    struct ifreq ifr;
    memset( &ifr,  0, sizeof(ifr));
    strcpy( ifr.ifr_name, "eth1" );
    if( ioctl( sockfd, SIOCGIFINDEX, &ifr ) < 0 ) {
        perror("ioctl:ifreq");
        return 1;
    }
    printf( "if index: %d\n", ifr.ifr_ifindex );

    struct sockaddr_ll sa;
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = htons( ETH_P_ALL ); 
    sa.sll_ifindex = ifr.ifr_ifindex;

    if( bind( sockfd, (struct sockaddr*) &sa, sizeof(sa) ) < 0 ) {
        perror( "bind" );
        return 1;
    }

    while(1) {
        fd_set fds, readfds;
        FD_ZERO( &readfds);
        FD_SET( sockfd, &readfds );
        memcpy( &fds, &readfds, sizeof(readfds));
        if( select( sockfd+1, &fds, NULL, NULL, NULL ) < 0 ) {
            perror("select");
            return 1;
        }

        if( FD_ISSET( sockfd, &fds ) ) {
            char buffer[1024];
            struct sockaddr_ll sll;
            int sll_size = sizeof(sll);
            int frame_len = recvfrom( sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &sll, &sll_size );
            if( frame_len < 0) {
                perror("recvfrom");
                return 1;
            }
            printf("received len:%d\n", frame_len );            
            if( frame_len == 5 ) {
                printf("data: %c %c %c %c %c\n",
                       buffer[0], buffer[1], buffer[2], buffer[3], buffer[4] );
            }
                

        }
    }
    
    return 0;
}
