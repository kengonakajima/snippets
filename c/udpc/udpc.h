#ifndef _UDPC_H_

#define _UDPC_H_

int UDPCInit(unsigned short port);
void UDPC(void);
int UDPCAccept(void);
int UDPCConnect( unsigned long ipaddr , unsigned short port );
int UDPCWrite( int vfd , char *buf , int len );
int UDPCRead( int vfd , char *buf , int len );
int UDPCClose( int vfd );


#define ENOLISTEN		909			/* listenしてないぞ */
#define ENOACCEPTWAIT	1010		/* accept waitの状態になっているソケットがあるはずなのにない。 */
#define ESOCKETERR		505			/* socket(できなかった。) */
#define ENOMORESOCKET	606			/* これ以上ソケットをつくれないぞ */
#define EBADFDNUM		101			/* fdの値が変である */
#define EBADDATALEN		202			/* パケット送信できるデータのサイズを越えてるか負なので送信しなかった */
#define EBUFFERMAX		303			/* ソケットにためることができなかった。 */
#define ETIMEOUT		404			/* timeoutしてしまった。 */
#define ENOTINIT		2020		/* まだ初期化されてないぞ */
#define EALREADYUSE		3030		/* すでにそのポートはつかわれている。 */
#define EREADNODATA			4004		/* データがないのにreadしようとした。 */




#endif	/* _UDPC_H_ */
