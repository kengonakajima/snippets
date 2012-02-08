/*
  ñ��β��������Τ��٤ϡ�
  �ϥå��� -> �ꥹ�� �Ǥ��
  
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>



typedef struct apent_tag
{
    char *p;        /* dbbuf �Τ�����֤ؤΥݥ��� */
    int count;
    int hash;
    struct apent_tag *next;
} apent;


#define HASHCOUNT ( 500000  )      /* �ϥå�����ͤμ��ࡣ�礭��������᤯
                                 �ʤ����
                                 1000��������åפ��ޤ��äƤ���
                                 400���� 26�á��Ĥ�������ʤ�
                                 200���� 25�� �Ĥ�������ʤ�
                                 100���� 23�á��Ĥ�������ʤ�
                                 10���� 22�á��Ĥ�������ʤ�(55297)
                                 1���� 27�äĤ�������ʤ�(9650)
                                 5000:  29sec    �Ĥ�������
                                 2500:  31��  �Ĥ�������
                                 1000:  47��  �Ĥ������ä�

                                 drink���Ȥ��礦��2�ܤˤʤ롣 11�á�
                                 100���� 10sec hash�ʤ����롼�� 6��
                                 200���� 10sec
                                 ���꤬�������Τ�¿�����Ƥ⤢�ޤ��٤�
                                 �ʤ��

                                 
                                 */
apent *aptop[HASHCOUNT];


#define DBSIZE ( 1024 * 1024 )

/*ñ������Τ�����Ƥ����Хåե�����������1ñ�줢����8�Х��Ȥ��� */
char dbbuf[DBSIZE * 8 ];
apent apbuf[DBSIZE];        /* ñ��θĿ����� */


int call_count = 0;
time_t start_tm;

/*  ���ΥХåե����餦��Ĺ��������ʸ�������¸����Τ�
    strlen + 1 ����������Ϳ���뤳�ȡ�
 */
static char *nexp = dbbuf;
static int lastapbufind = 0;
apent *getnextapent( int len )
{
    apent *a = & apbuf[lastapbufind++];

    a->p = nexp;
    nexp += len;

    return a;
}

unsigned int hashpjw ( char* s )
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
/*
  ñ�����Ͽ���� 
 */
int htop_usage = 0;
int add_word( char *w )
{
    unsigned int h = hashpjw( w ) % HASHCOUNT;
    apent *now_top_ap;

    call_count ++;
    if( ( call_count % 5000 ) == 0 ){
        fprintf( stderr ,"C:%d Usage:%d %d\n",call_count , htop_usage ,
                 time(NULL) - start_tm );
    }

    if( aptop[h] == NULL ){
        /* �ޤ����Υϥå����ͤ���ñ�����Ͽ����Ƥʤ��Τ�
           ��Ͽ���� */
        int l = strlen( w );
        apent *ape = getnextapent( l + 1);
        aptop[h] = ape;
        ape->count = 1;
        ape->next = NULL;
        ape->hash = h;
        memcpy( ape->p , w , l + 1 );
//        fprintf( stderr , "%d new [%s]\n", call_count , w );
        htop_usage ++;
        return;
    }

    /* 1�İʾ���Ͽ����Ƥ��Τǥ�󥯤򤿤ɤ� */

    now_top_ap = aptop[h];
    for(;;){
        if( now_top_ap->hash == h && strcmp( now_top_ap->p , w ) == 0 ){
            now_top_ap->count ++;
//     fprintf( stderr, "add count to %d: [%s]\n", now_top_ap->count,w );
            return;
        } else {
            if( now_top_ap->next == NULL ){
                /* ��󥯤κǸ�ޤǤ����ΤǤ���ñ�����Ͽ�����
                   ���ʤ��ä��Τ��� */
                int l = strlen( w );                
                apent *ape = getnextapent( l + 1 );
                now_top_ap->next = ape;
                ape->next = NULL;
                memcpy( ape->p , w , l + 1 );
                ape->count = 1;
                ape->hash = h;
            } else {
                now_top_ap = now_top_ap->next;
            }
        }
    }

    
}






    


void rout( char*nm )
{
    int i;
    FILE *fp=fopen( nm , "w");
    for(i=0;i<DBSIZE;i++){
        if( apbuf[i].p ){
            fprintf( fp, "%s %d\n", apbuf[i].p , apbuf[i].count );
        }
    }
    fclose(fp);
    fprintf( stderr , "CALL:%d\n", call_count);
}

void r( char *aho)
{
    char fuck[10000];
    FILE*fp=fopen(aho,"r");
    while(fgets(fuck,sizeof(fuck),fp)){
        fuck[strlen(fuck)-1]=0;
        add_word( fuck );
    }
    fclose(fp);
}

void siginth(int a)
{
    exit(0);
}
int main( int argc, char **argv )
{
    
    if( argc == 1 ){
        fprintf(stderr ,"usage: infile outfile]\n" );
        exit(0);
    }
    
    time ( &start_tm );
    signal( SIGINT,siginth );

    r(argv[1]);

    rout(argv[2]);

}


