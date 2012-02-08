

#ifndef _TSRPC_CLIENT_H_

#define _TSRPC_CLIENT_H_

#include "tsrpc.h"




/* 関数のプロトタイプ */
int remotesum2( CLIENT *ch , char *arg1 , int arg2[10] );


CLIENT * tsrpcCreateClient( char *hostname , int port );
int tsrpcDestroyClient( CLIENT* );
char* tsrpcGetLastError( CLIENT *ch , char *err );
void call_tsrpctcp( CLIENT *ch , char *callbuf , char *retbuf );
void tsrpcClientReadLine( CLIENT *ch , char*retbuf );



#endif     /* ifndef _TSRPC_CLIENT_H_ */
