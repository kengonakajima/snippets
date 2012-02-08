
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/errno.h>
#include <sys/time.h>





/* エラー番号 */

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




int uproc(void);
int ureadable( int fd );
int uconnect( int fd , unsigned long addr , unsigned short port );
int usocket_serv(int index );
int uaccept( int fd , unsigned long *cliaddr,  unsigned short *cliport );
int uwrite( int fd , char *data , int len );
int uread( int fd, char *data , int len );
void ustat(void);

