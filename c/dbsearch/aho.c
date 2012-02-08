/*
  ̾��: UID �Ȥ����ǡ����١�����1000����ʬ����Ȥ��롣
  10M�ͤʤΤ�
  hogehoge 10084778
  �Ȥ������������20�Х��Ȥǥե�����ˤ���� 200M�Ǥ��롣
  ������ˡ���������ʿ�Ѥ���  40�ͤ�Ʊ��̾���Υ桼��������Ȥ��롣

  ���θ�����ɤ�����Τ���
  ñ���RDB���ǡ����١���������ΤϽ�ʬ��®�ˤǤ��뤳�Ȥ�Ƚ�����Ƥ��뤬��
  ���ǡ������鸡������ΤϤ�Ĥ������� test/rdb �Υ����ɤ����
  ���롣

  �ºݤˤ�1000����(40MByteʬ)�Υ����ƥ���UID���������ɬ�פ����롣

  uid
  nick�ؤ�index
  email�ؤ�index
  ...�ؤ�index
  �Ȥ����褦��ʣ���ξ�����Ĺ�¤�Τ�������ơ�����Ǥ⤳�������
  �������˻��äƤ���ɬ�פ����롣�ǡ���������������Ϥ���
  �����������������ɬ�פ����롣�褦�˻פ��롣
  �����nick�����ˤ��ܤäƤ��󤬤��롣

  ����Ū�ˤϡ�Ʊ��nick����Ŀͤ���󥯤ǤĤʤ��äƤ��ơ�
  hash -> ���Υ�󥯤Υȥå� -> �롼�� �Ǹ����Ǥ�����50�ͤ��Ƥ�50�롼��
  �ǺѤࡣ �����ޤǤ���������

  Nick�Ȥ�ñ��ʤΤǡ�ñ�쥨��ȥ꤬UID��󥯤ؤΥȥåפˤʤäƤ����
  ���ʤ�褤��
  First-name �� last-name �⡢ñ��Ǥ��ꡢ�ǽ�Ū�ˤ� UID �򸡺��������Τǡ�
  UID��󥯤��������С����Υ�󥯤򸡺����Ƥ���������UID��ȯ������
  ���Ȥ���ǽ�äݤ���
  unsigned int UID
  int type
  unsigned int next
  �Ȥ���12�Х��ȤΥ�󥯤�Ĥ���Ф褤��1000���ͤ�120MB�Ȥʤ롣
  
 */
/*
  ñ��β��������Τ��٤ϡ�
  �ϥå��� -> �ꥹ�� �Ǥ��
  
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>


typedef enum{
    NICK = 1,
    EMAIL=2,
    FNAME=3,
    LNAME=4,
} DBTYPE;

typedef struct uent_tag
{
    unsigned int uid;
    struct uent_tag *next;
} uent;

#define MAXUENT ( 1024 * 1024 )     /* ���꤬�����ʤ��ΤǤǤ������᤻�ʤ� */
uent ubuf[MAXUENT];


typedef struct apent_tag
{
    char *p;        /* dbbuf �Τ�����֤ؤΥݥ��� */
    DBTYPE type;
    struct apent_tag *next;
    uent *utop;
} apent;


#define HASHCOUNT ( 500000  )      /* �ϥå�����ͤμ��ࡣ�礭��������᤯
                                 �ʤ������5���Ǥ褤���Ȥ�Ƚ�� */
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

unsigned int hashpjw( char* s )
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

int last_uent_index = 0;
uent *getnewuent( void )
{
//    fprintf( stderr ,"getnewuent:%d\n",last_uent_index );
    return & ubuf[ last_uent_index ++ ];
}


/*
  ñ�����Ͽ���� 
 */
int htop_usage = 0;
int add_u( unsigned int uid , char *nk, char *fn , char *ln , char *em  )
{
    uent *ue;

    /* Nickname ������ɬ�� */
    if( nk[0] == 0 ) return -1;


    
    ue = getnewuent();
    ue->uid = uid;
    ue->next = NULL;

    add_word( nk , NICK , ue );
    
    if( fn[0] ){
        ue = getnewuent();
        ue->uid = uid;
        ue->next = NULL;
        add_word( fn , FNAME , ue);
    }

    if( ln[0] ){
        ue = getnewuent();
        ue->uid = uid;
        ue->next = NULL;
        add_word( ln , LNAME , ue );
    }

    if( em[0] ){
        ue = getnewuent();
        ue->uid = uid;
        ue->next = NULL;
        add_word( em ,EMAIL , ue);
    }
}
int ii_max = 1000;
int add_word( char *w , DBTYPE type , uent *up )
{
    unsigned int h = hashpjw( w ) % HASHCOUNT;
    apent *now_top_ap;

    
    call_count ++;
    if( ( call_count % 5000 ) == 0 ){

//    {    
        fprintf( stderr ,"C:%d Usage:%d %d\n",call_count , htop_usage ,
                 time(NULL) - start_tm );
    }

    if( aptop[h] == NULL ){
        /* �ޤ����Υϥå����ͤ���ñ�����Ͽ����Ƥʤ��Τ�
           ��Ͽ���� */
        int l = strlen( w );
        apent *ape = getnextapent( l + 1);
        aptop[h] = ape;
        ape->next = NULL;
        ape->type = type;
        memcpy( ape->p , w , l + 1 );
        htop_usage ++;

        /* �Ϥ���Ƥν�����ʤΤ�  utop ������ */
        ape->utop = up;
        return;
    }

    /* 1�İʾ���Ͽ����Ƥ��Τǥ�󥯤򤿤ɤ� */

    now_top_ap = aptop[h];
    for(;;){
        /* �����פ�ʸ���󤬴����˰��פ�������� */
        if( type == now_top_ap->type &&
            strcmp( now_top_ap->p , w ) == 0 ){
            /* UID����ȥ�ꥹ�ȤκǸ�� */
            uent *ut;
            int i=0;
            ut = now_top_ap->utop;            
            for(;;){
                i++;
                if(ut->next == NULL){
                    ut->next = up;
//                    fprintf( stderr , "appended\n");
                    break;
                }
                ut = ut->next;
            }

            if( i > ii_max ){
                ii_max = i;
                fprintf( stderr , "uent appnd i:%d H:%d %s\n",i,(int)h,w);
            }
            return;
        } else {
            if( now_top_ap->next == NULL ){
                /* ��󥯤κǸ�ޤǤ����ΤǤ���ñ�����Ͽ�����
                   ���ʤ��ä��Τ��� */
                int l = strlen( w );                
                apent *ape = getnextapent( l + 1 );
                
                now_top_ap->next = ape;
                ape->next = NULL;
                ape->type = type;
                memcpy( ape->p , w , l + 1 );
                ape->utop = up;
                return;
            } else {
                now_top_ap = now_top_ap->next;
            }
        }
    }
    
}

/*
  ʸ����ȥ����פ��鸡������
 */  
void search( DBTYPE type , char *s )
{
    unsigned int h = hashpjw( s ) % HASHCOUNT;
    apent *now_top;

    if( aptop[h] == NULL ){
        fprintf( stderr, "no user found\n");
        return;
    }

    /* ������ */
    now_top = aptop[h];
    for(;;){
        if( now_top == NULL ) break;
        if( type == now_top->type &&
            strcmp( s , now_top->p ) == 0 ){
            /* found */
            uent *utop = now_top->utop;
            for(;;){
                if( utop == NULL ){
                    break;
                } else {
                    fprintf( stderr , "%u\n", utop->uid );
                    utop = utop->next;
                }
            }
        }
        now_top = now_top->next;
    }
}

                


void rout( char*nm )
{
    int i;
    FILE *fp=fopen( nm , "w");
    for(i=0;i<DBSIZE;i++){
        if( apbuf[i].p ){
            uent *u;
            fprintf( fp, "%s TYPE:%d U:",
                     apbuf[i].p , apbuf[i].type );
            u = apbuf[i].utop;
            for(;;){
                if(u==NULL)break;
                fprintf( fp , " %u" , u->uid );
                u = u->next;
            }
            fprintf( fp , "\n");
        }
    }
    fclose(fp);
//    fprintf( stderr , "CALL:%d\n", call_count);
}

unsigned int ahouid = 1000000;

void r( char *aho)
{
    char fuck[10000];
    char fuck2[10000];
    char fuck3[10000];
    char fuck4[10000];
    
    FILE*fp=fopen(aho,"r");
        
    while(1){
        fuck[0] = fuck2[0] = fuck3[0] = fuck4[0] = 0;

        if(fgets(fuck,sizeof(fuck),fp)==0)break;
/*        if(fgets(fuck2,sizeof(fuck2),fp)==0)break;
        if(fgets(fuck3,sizeof(fuck3),fp)==0)break;
        if(fgets(fuck4,sizeof(fuck4),fp)==0)break;*/
        fuck[strlen(fuck)-1]=0;
/*        fuck2[strlen(fuck2)-1]=0;
        fuck3[strlen(fuck3)-1]=0;
        fuck4[strlen(fuck4)-1]=0;*/

//        fprintf( stderr, "%s\n",fuck);
//        add_u( ahouid++ , fuck ,fuck2,fuck3,fuck4);
        add_u( ahouid++ , fuck ,"","","");
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

    
//    search( EMAIL , "EEnm" );
}
