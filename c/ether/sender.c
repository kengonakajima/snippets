#include <stdio.h>
#include <sys/socket.h> // PF_PACKET, SOCK_RAW
#include <net/ethernet.h> // ETH_P_ALL
#include <net/if.h> // struct ifreq
#include <sys/ioctl.h> // ioctl, SIOCGIFINDEX
#include <netpacket/packet.h> // struct sockaddr_ll
#include <net/if_arp.h>
#include <netinet/ip.h> // IPPROTO_RAW

#include <string.h> // memset
#include <errno.h>

unsigned char dest_mac[6] = {
    //        0xb0, 0xc7, 0x45, 0x69, 0xc0, 0x24 // router
    //    0xb8, 0xf6, 0xb1, 0x14, 0xdb, 0x67 // retina mbp  
    //    0x08, 0x00, 0x27, 0xa6, 0x34, 0x8a // VM on osx mbp (centos64local)
         0x90, 0x27, 0xe4, 0xfd, 0xa3, 0x17 // mbp win7
    
    //     0x08, 0x00, 0x27, 0x3c, 0xae, 0x12 // VM on win7 mbp
               
};
unsigned char src_mac[6] = {
    //0xb0, 0xc7, 0x45, 0x69, 0xc0, 0x24 // router
    // 0xb8, 0xf6, 0xb1, 0x14, 0xdb, 0x67 // retina mbp
    0x08, 0x00, 0x27, 0xa6, 0x34, 0x8a // VM(centos64local)    
};

int main( int argc, char *argv ) {
    int sockfd = socket( PF_PACKET, SOCK_RAW, IPPROTO_RAW ); //htons( ETH_P_ALL ));   // need root
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

    int cnt=0;
    int i;
    
    while(1){
        struct sockaddr_ll sll;

        memset( &sll, 0, sizeof(sll));
        sll.sll_family = AF_PACKET;
        sll.sll_protocol = 0;// htons( ETH_P_802_3 );// 何でもいい
        sll.sll_halen = ETH_ALEN;
        sll.sll_ifindex = ifr.ifr_ifindex;
        //sll.sll_pkttype = PACKET_BROADCAST;
        sll.sll_pkttype = PACKET_OTHERHOST;
        sll.sll_hatype = ARPHRD_ETHER;
        for(i=0;i<6;i++) sll.sll_addr[i] = src_mac[i];

        int sendlen;
        char buffer[6+6+2+5+4];
        memset( buffer, 0, sizeof(buffer));


        
        for(i=0;i<6;i++) buffer[0+i] = dest_mac[i];
        for(i=0;i<6;i++) buffer[6+i] = src_mac[i];
        
        buffer[12] = 0x05;
        buffer[13] = 0x00;
  

        buffer[14] = 'h';
        buffer[15] = 'e';
        buffer[16] = 'l';
        buffer[17] = 'l';
        buffer[18] = 'o';
        
        buffer[19] = 0;
        buffer[20] = 0;
        buffer[21] = 0;
        buffer[22] = 0;        
            
        sendlen = sendto( sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &sll, sizeof(sll));
        if( sendlen < 0 ) {
            perror("sendto");
            return 1;
        }
        printf( "sendlen:%d\n", sendlen );
        usleep(500*1000);
        //        sleep(1);
        printf("loop %d..\n", cnt );
        cnt ++;
    }
    printf("finished\n");

    return 0;
}


