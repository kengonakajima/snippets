#include <stdio.h>
#include <sys/socket.h> // PF_PACKET, SOCK_RAW
#include <net/ethernet.h> // ETH_P_ALL
#include <net/if.h> // struct ifreq
#include <sys/ioctl.h> // ioctl, SIOCGIFINDEX
#include <netpacket/packet.h> // struct sockaddr_ll
#include <net/if_arp.h>
#include <netinet/ip.h> // IPPROTO_RAW

#include <string.h> // memset
#include <ctype.h> // tolower
#include <unistd.h>
#include <errno.h>


void getMacAddress( unsigned char *out, char *in ) {
    if( strlen(in) != ( (6*3)-1 ) ) { perror("mac format"); }
    int i;
    for(i=0;i<6;i++) out[i] = strtol(in+(i*3), NULL, 16 );
}

int main( int argc, char **argv ) {
    if( argc != 3 ) { printf("arg: sender SRCMAC DESTMAC\n"); return 1; }

    unsigned char src_mac[6], dest_mac[6];

    getMacAddress( src_mac, argv[1] );
    getMacAddress( dest_mac, argv[2] );    
                 
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
        char buffer[6+6+2+46+4];
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
        for(i=19;i<14+46;i++) buffer[i] = 0;
        
        buffer[60] = 0;
        buffer[61] = 0;
        buffer[62] = 0;
        buffer[63] = 0;        
            
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


