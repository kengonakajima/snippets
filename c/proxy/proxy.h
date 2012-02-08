#ifndef _PROXY_H_
#define _PROXY_H_



/*
 * �ޥ���˰��ڰ�¸���ʤ�(32�ӥåȥͥ��ƥ��֤Ǥ��뤳�Ȥϡ�����)�Υ�����
 * ����
 *
 */


/* �ץ�����ư���롣���δؿ��Ͻ�λ����Ȥ��ʳ������äƤ��ʤ���
   ���ｪλ������0��������ʤ��Ф�����1�򤫤����ޤ��礦��
   ���δؿ��Ϥ��ʤ餺������ʤ���Фʤ�ʤ��Τ���
 */
int startProxy( int argc , char **argv );

/* HTTP ��Ϣ�Ρ��ޥ����¸���ʤ����� */
void getHTTPRequest( char *src , char *out , int maxlen );
void getHTTPRequestURL( char *src , char *out , int maxlen );
void getHTTPRequestMethod( char *src , char *out , int maxlen );
void getHTTPRequestHostname( char *src , char *out , int maxlen );
void getHTTPRequestScheme(  char *src , char *out , int maxlen );
void getHTTPRequestFilename( char *src , char *out , int maxlen );
void getHTTPHost( char *src , char *out, int maxlen );

/* �Τ֤����Υ롼���󡣥ޥ����¸���ʤ������� */
int getStringFromIndexWithDelim( char* src ,char* delim ,int index,
                                 char* buf , int buflen);




#endif
