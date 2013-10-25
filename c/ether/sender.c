#include <stdio.h>
#include <sys/socket.h> // PF_PACKET, SOCK_RAW
#include <net/ethernet.h> // ETH_P_ALL
#include <net/if.h> // struct ifreq
#include <sys/ioctl.h> // ioctl, SIOCGIFINDEX
#include <netpacket/packet.h> // struct sockaddr_ll
#include <net/if_arp.h>

#include <assert.h>
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

    while(1){
        struct sockaddr_ll sll;

        sll.sll_family = AF_PACKET;
        sll.sll_protocol = htons( ETH_P_ALL );
        sll.sll_halen = ETH_ALEN;
        sll.sll_ifindex = ifr.ifr_ifindex;
        sll.sll_pkttype = PACKET_BROADCAST;
        sll.sll_hatype = ARPHRD_ETHER;
        
        int sendlen;
        char buffer[5] = { 'h', 'e', 'l', 'l', 'o' };
        
        sendlen = sendto( sockfd, buffer, 5, 0, (struct sockaddr*) &sll, sizeof(sll));
        if( sendlen < 0 ) {
            perror("sendto");
            return 1;
        }
        printf( "sendlen:%d\n", sendlen );
        sleep(1);
        printf("loop..\n");
    }


    return 0;
}


