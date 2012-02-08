/*
  rdb�ߤ����ʤ�ΤΥƥ��Ȥ�Ѹ��ʸ�Ϥ��ɤߤ����ñ��index��դäƤ��ä���
  ���Ƥ�äƤߤ롣
  
  ʸ�����ɽ�����äơ�����ˤ� index �ǥ��������Ǥ��롣
  ����򤮤ä���Ȥ�����ˡ���ʸ�������Ƭ���ɥ쥹�����줿�����
  ���ʤ���������äƤ���(���Υ���Τۤ��������ʤ������Ȥ������Ȥǡ�
  �ʤˤ���1�Ĥ�����4�Х��ȤΥ���Ǥ�����ʤ���)
  

  �ºݤα��ѤǤϡ�������ʹ��ܤ��Ȥ�ɽ�����������
  
 */

#include <stdio.h>
#include <signal.h>
#include <time.h>

#define MAXDB  (  1024 * 1024 ) /* ñ���100������ޤ� */

typedef struct 
{
    char *p;      /* ���ƥХåե��Τɤ��� */
    int len;
    int count;
    int hash;
} pent;

pent plist[MAXDB];       /*NULL �Ϥ���ʾ�ʤ��Ȥ������ȡ�
                                  RDB �ι��ܤ������뤳�ȤϤ��äƤ⸺��
                                  ���ȤϤʤ��Τ��� */
char dbbuf[MAXDB * 10];         /*ʿ��10ʸ���Ȥ������������Ȥ�ط�����
                                  ���뤬��ʬ�ˤȤ��OK�Ȥ������Ȥǡ� */

#define QBUFSIZE ( 1023*1023*7  )  /* ��®���Τ���Υϥå���->plist�ؤ�
                                      index ɽ */
int qbuf[QBUFSIZE];

int call_count = 0;

/*
  �ϥå��女���ɤ���ɽ�Υ���ȥ���Ȥ�� strcmp ����ΤǥҥåȤ��ʤ�
  ���Ϻ����������Ƥ���®�����ʡ�
  
 */

/* pjw �����ǥϥå�����Ȥ�롣
 40�ä�53��/48��Q ���ä������hash�Ǥ�40�ä� 25��/16��Q ���ä���
 ��ǽ��������*/

int hashpjw ( char* s )
{
    char *p;
    unsigned int h= 0 ,g;
    for( p = s ; *p ; p ++ ){
        h = ( h<< 4 ) + (*p);
        if( (g = h & 0xf0000000) != 0){
            h = h ^ (g>>24);
            h = h ^ g;
        }
    }
    return h;
}


/* �ϥå��女���ɤ��Ȥ�� */
int hash( char *s )
{
    int h = 0;
    if( s[0] != 0 ) h += s[0];
    if( s[1] != 0 ) h += s[1] * 256;
    if( s[2] != 0 ) h += s[2] * 256* 256;
    if( s[3] != 0 ) h += s[3]
                        * 256* 256* 256;
    return h;
}

    int qbhit = 0 ;
time_t start_tm;

void siginth(int a)
{
    fprintf( stderr , "QBHIT:%d\n", qbhit );
    exit(0);
}
    
/*
  ñ����ɲ�
 */
int add_word( char *w )
{
    int i;
    int l = strlen( w );
    char *p;
    int h = hashpjw( w );
    call_count ++;

    if( ( call_count % 1000 ) == 0 ){
        fprintf( stderr , "C:%d Q:%d %d\n",call_count,qbhit ,
                 time(NULL)-start_tm );
    }

    /* �ޤ� qbuf ��Ĥ������Ȥ��Ƥߤ� */
    {
        int qhash = h % QBUFSIZE;
        if( qbuf[qhash] > 0 && strcmp(plist[qbuf[qhash]].p,w)==0){
            qbhit++;
            plist[qbuf[qhash]].count ++;
//            fprintf( stderr ,"C:%d Q:%d [%s]\n",call_count,qhash, w);
            return 1;
        }
    }
    
    p = dbbuf;
    for(i=0;i<MAXDB;i++){
        if( plist[i].p == NULL ){
            memcpy( p , w , l + 1 );
            
            plist[i].p = p;
            plist[i].len = l;
            p += l + 1;
            plist[i].count++;
            plist[i].hash = h;
//            fprintf(stderr ,"C:%d I:%d [%s]\n", call_count,i,
//                    plist[i].p );
            qbuf[ h % QBUFSIZE ] = i;
            return 1;
        } else {
            if( plist[i].hash == h && strcmp( plist[i].p , w ) == 0 ){

                plist[i].count ++;

                if( plist[i].count < 10 ){
/*                    fprintf(stderr ,"C:%d I:%d [%s] %d\n",
                            call_count, i ,
                            plist[i].p,
                            plist[i].count );
                            */                            
                }
                return 1;
            }            
            p += plist[i].len + 1;
        }
    }
    return 0;
}

void rout( char*nm )
{
    int i;
    FILE *fp=fopen( nm , "w");
    for(i=0;i<MAXDB;i++){
        if( plist[i].p ){
            fprintf( fp, "%s %d\n", plist[i].p , plist[i].count );
        }
    }
    fclose(fp);
    fprintf( stderr , "QBHIT:%d CALL:%d\n", qbhit , call_count);
}

void r( char *aho)
{
    char fuck[10000];
    FILE*fp=fopen(aho,"r");
    
    while(fgets(fuck,sizeof(fuck),fp)){
        fuck[strlen(fuck)-1]=0;

//        fprintf( stderr , "<%s>", fuck);
        add_word( fuck );
    }
    fclose(fp);
}
void s( char *aho)
{
}

int main( int argc, char **argv )
{
    
    if( argc == 1 ){
        fprintf(stderr ,"usage: [-i infile] [-s searchword]\n" );
        exit(0);
    }
    time ( &start_tm );
    signal( SIGINT,siginth );
    /* �ǽ�ΰ����� -i �ΤȤ��� infile ����ñ�����*/
    if( strcmp( argv[1] , "-i" ) == 0 ){
        r(argv[2]);
        rout(argv[3]);
    } else {
        s(argv[2]);
    }
}


    
