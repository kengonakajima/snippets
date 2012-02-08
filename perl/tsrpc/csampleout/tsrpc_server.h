
#ifndef _TSRPC_SERVER_H_

#define _TSRPC_SERVER_H_
#include "tsrpc.h"


/*   プロトコルファイルから生成される関数のプロトタイプ */
int remotesum2( char *str , int arg1[10] );




void tsrpcServerDispatcher( char *buffer , int sockfd );
SERVER* tsrpcCreateServer( char *local , int port ,
					void(*loopfunc)(void));
void tsrpcServerMainLoop( SERVER*sv );
void tsrpcDestroyServer( SERVER*s );



#endif		/* ifndef _TSRPC_SERVER_H_ */
