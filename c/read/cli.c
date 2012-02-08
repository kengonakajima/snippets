#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

/*
   開発中にはクライアントがないため、実験用のもの。
   cursesを使う。
   地形の表示は、ある、なし的な単純なものにする。
   
*/

/*
   これは、クライアントが持つべきデータなので、サーバーとは関係ない。
   javaのコーディングの参考になるだろう。

*/
/****************************************************************
	cursesバージョンの操作方法


	hjkl  	4方向移動 (原則として、4方向しか動けない )
	a	攻撃

	


	
****************************************************************/

#define IPADDRESS "192.168.1.11"
#define PORT 9000

int main(int argc , char **argv )
{
	int sockfd ;
	struct sockaddr_in serv_addr;
	int siz;
	
	if( argc== 1 ){
		fprintf(stderr,"usage: ctest siz \n");
		exit(1);
	}

	siz = atoi( argv[1] );
	
	   /* ソケットを開く */
	
	bzero(( char *) &serv_addr , sizeof( serv_addr ));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr( IPADDRESS );
	serv_addr.sin_port = htons( PORT );

	if( ( sockfd = socket( AF_INET , SOCK_STREAM , 0 )) < 0 ){
	    	fprintf(stderr,"client:socket error.\n");
		exit(1);
	}

	if( connect( sockfd ,( struct sockaddr *) & serv_addr , sizeof( serv_addr ))< 0 ){
	    	fprintf(stderr , "client: connect error.\n");
		exit(1);
	}
	{
		int data[32768];
		int i;
		for(i = 0 ; i < siz ; i++ ) data[i] = 0;
		for(i = 0; ;i++ ){
			int n;
			sleep(1);
			data[0] = i;
			data[siz-1] = -i;
			n = write(sockfd , data , ( siz * 4 ) );
			fprintf(stderr, "client:wrote %dbytes.\n" , n);
				n = write(sockfd , data , ( siz * 4 ) );
			fprintf(stderr, "client:wrote %dbytes.\n" , n);
			
		}
	}

	
}
