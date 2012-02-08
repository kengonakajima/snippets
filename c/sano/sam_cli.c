//*************************************************:
//client.c
//*************************************************

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define PORT (u_short)10000
#define BUF_LEN 100

char hostname[]="wakkannai";
char buf[BUF_LEN];

void main()
{
		struct hostent *servhost;
		struct sockaddr_in	 server;
		int s;

		printf ("jlahflahffhkfg");
		//相手のアドレス
		servhost = gethostbyname(hostname);
		bzero((char *)&server,sizeof(server));
		server.sin_family = AF_INET;
		server.sin_port = PORT;
		bcopy (servhost->h_addr,(char *)&server.sin_addr,servhost->h_length);

		//ソケットを獲得する。
		s= socket(AF_INET,SOCK_STREAM,0);

		connect(s,(struct sockaddr *)&server,sizeof(server));


		write(1,"待ちなさい\n",11);
		//対話ループ
		do
		{
		int n;
		n=read(s,buf,BUF_LEN);
		write(1,buf,n);
		n=read(0,buf,BUF_LEN);
		write(s,buf,n);
		}
		while (strncmp(buf,"quit",4) !=0);

		close(s);
}
