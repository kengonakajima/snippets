//*************************************************:
//severe.c
//*************************************************

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>


#define PORT (u_short)10000
#define BUF_LEN 100

char hostname[]="drink";
char buf[BUF_LEN];

int main()
{
		struct hostent *myhost;
		struct sockaddr_in me;
		int	s_waiting , s;

		//��ʬ�Υ��ɥ쥹��ͣŤȸ����ѿ��ˤĤ��������
		myhost = gethostbyname(hostname);
		bzero((char *)&me,sizeof(me));
		me.sin_family = AF_INET;
		me.sin_port = htons(PORT);
		bcopy (myhost->h_addr,(char *)&me.sin_addr,myhost->h_length);
		//�����åȤ����������ʬ�Υ��ɥ쥹���ӤĤ���
		s_waiting = socket(AF_INET,SOCK_STREAM,0);
		
		if( bind(s_waiting ,(struct sockaddr *) &me , sizeof(me) ) < 0 ){


        
        printf("ahhaoerror: %s\n" , strerror(errno));
        
        }
			
		//��³�γ�Ω
		listen(s_waiting,1);
		s=accept(s_waiting,NULL,NULL);

        printf("Accept returns: %d\n" , s );
        
		close(s_waiting);
		
		write(1,"�������Ĥ������� \n",17);
		//���äΥ롼��
		do
		{
		int n;
		n=read(0,buf,BUF_LEN);
		write(s,buf,n);
		n=read(s,buf,BUF_LEN);
		write(1,buf,n);
		}
		while (strncmp(buf,"quit",4) !=0);
		
		close(s);
}