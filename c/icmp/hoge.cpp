#include <stdio.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>


/*
 * チェックサムを計算する関数です。
 * ICMPヘッダのチェックサムフィールドを埋めるために利用します。
 * IPヘッダなどでも全く同じ計算を利用するので、
 * IPヘッダのチェックサム計算用としても利用できます。
 */
unsigned short
checksum(unsigned short *buf, int bufsz)
{
  unsigned long sum = 0;

  while (bufsz > 1) {
    sum += *buf;
    buf++;
    bufsz -= 2;
  }

  if (bufsz == 1) {
    sum += *(unsigned char *)buf;
  }

  sum = (sum & 0xffff) + (sum >> 16);
  sum = (sum & 0xffff) + (sum >> 16);

  return ~sum;
}

/* main 文はここからです。*/
int
main(int argc, char *argv[])
{
  int sock;
  struct icmphdr hdr;
  struct sockaddr_in addr;
  int n;

  char buf[2000];
  struct icmphdr *icmphdrptr;
  struct iphdr *iphdrptr;

  //  if (argc != 2) {
  //    printf("usage : %s IPADDR\n", argv[0]);
  //    return 1;
  //  }

  //  addr.sin_family = AF_INET;
  //  addr.sin_addr.s_addr = inet_addr(argv[1]);

  /* RAWソケットを作成します */
  sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  memset(&hdr, 0, sizeof(hdr));

  /* ICMPヘッダを用意します */
  hdr.type = ICMP_ECHO;
  hdr.code = 0;
  hdr.checksum = 0;
  hdr.un.echo.id = 0;
  hdr.un.echo.sequence = 0;

  /* ICMPヘッダのチェックサムを計算します */
  hdr.checksum = checksum((unsigned short *)&hdr, sizeof(hdr));

  /* ICMPヘッダだけのICMPパケットを送信します */
  /* ICMPデータ部分はプログラムを簡潔にするために省いています */

#if 0  
  n = sendto(sock,
             (char *)&hdr, sizeof(hdr),
             0, (struct sockaddr *)&addr, sizeof(addr));
  if (n < 1) {
    perror("sendto");
  }
#endif
  /* ICMP送信部分はここまでです*/
  /* ここから下はICMP ECHO REPLY受信部分になります */

  memset(buf, 0, sizeof(buf));

  /* 相手ホストからのICMP ECHO REPLYを待ちます */
  while(1) {
      n = recv(sock, buf, sizeof(buf), 0);
      if (n < 1) {
          perror("recv");
      }

      /* 受信データからIPヘッダ部分へのポインタを取得します */
      iphdrptr = (struct iphdr *)buf;

      /*
       * 本当はIPヘッダを調べて
       * パケットがICMPパケットかどうか調べるべきです
       */

      /* 受信データからICMPヘッダ部分へのポインタを取得します */
      icmphdrptr = (struct icmphdr *)(buf + (iphdrptr->ihl * 4));

      /* ICMPヘッダからICMPの種類を特定します */
      if (icmphdrptr->type == ICMP_ECHOREPLY) {
          printf("received ICMP ECHO REPLY\n");
      } else {
          printf("received ICMP %d\n", icmphdrptr->type);
      }
  }

  /* 終了 */
  close(sock);

  return 0;
}
