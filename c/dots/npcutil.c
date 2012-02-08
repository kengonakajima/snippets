#include <string.h>
#include "common.h"
#include "char.h"
#include "char_base.h"
#include "object.h"
#include "item.h"
#include "map_deal.h"
#include "npcutil.h"
#include "npc_door.h"
#include "npc_box.h"
#include "readmap.h"
#include "npccreate.h"


/*
 * NPC���ä�Ũ�����ѤˤĤ����������ؿ���������ޤ���
 *�äƤ���
 *
 *
 * �Ȥꤢ��������ɬ�פʤΤϡ�
 *
 *
 * o �᤯�Υץ쥤�䡼��õ��®���Ƥޤ�٤�ʤ���ˡ
 *
 * o ̵�̤˹��⤷�ʤ������ ����Ƚ��ؿ�
 *
 * o ��פ����餤NPC�ΰ�ư�Ѵؿ�
 *
 *
 * 
 */



/*
 * �ι�򤤤ä�Ϳ����
 *
 * int charindex : ������index
 * int titleindex : �ι�μ�������ꤹ��index
 *
 * ����������TRUE ���Ԥ�����false
 */
BOOL NPC_Util_AddOneTitle( int charindex, int titleindex )
{
    int i;
    Char *c ;
    
    if( !CHAR_CHECKINDEX( charindex ) )return FALSE;

    c = CHAR_getCharPointer( charindex );
    
    for(i=0;i<CHAR_TITLEMAXHAVE;i++){
        if( c->indexOfHaveTitle[i] == -1 ){
            c->indexOfHaveTitle[i] = titleindex;
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * �ι���äƤ뤫�ɤ�������٤�
 *
 * 0 :��äƤʤ�
 * 1 : ��äƤ�
 * �� : Error
 */
BOOL NPC_Util_HaveTitle( int charindex , int titleindex )
{
    int i;
    Char *c;
    if( !CHAR_CHECKINDEX( charindex ) )return -1;
        
    c = CHAR_getCharPointer( charindex );
    for(i=0;i<CHAR_TITLEMAXHAVE;i++){
        if( c->indexOfHaveTitle[i] == titleindex ){
            return TRUE;
        }
    }
    return FALSE;
}


/*
 * �դ��ĤΥ���åɤ��٤ꤢ�äƤ뤫�ɤ����򤷤�٤롣
 *
 * int x1 , y1 : �ҤȤĤ�
 * int x2 , y2 : �դ��Ĥ�
 *
 * �Ȥʤꤢ�äƤ��� TRUE , ��������ʤ��ʤ�FALSE
 */
BOOL NPC_Util_Nearby( int x1 , int y1, int x2 , int y2 )
{
    if( ABS( x1 - x2 ) <= 1 &&
        ABS( y1 - y2 ) <= 1 ){
        return TRUE;
    } else {
        return FALSE;
    }
}
/*
 * �դ��ĤΥ���餬�٤ꤢ�äƤ뤫�ɤ����򤷤�٤롣
 * �����ͤ�ߤ�Τǡ�Ʊ�����֤Ǥ��٤����ǧ�ꤵ���
 *
 * int ind1 , ind2 : �դ��ĤΥ���饯����index
 *
 *
 * �Ȥʤꤢ�äƤ��� TRUE , ��������ʤ��ʤ�FALSE
 */
BOOL NPC_Util_CharNearby(int ind1,int ind2)
{
    if( !CHAR_CHECKINDEX(ind1)) return FALSE;
    if( !CHAR_CHECKINDEX(ind2)) return FALSE;

    if( CHAR_getInt( ind1 , CHAR_FLOOR ) !=
        CHAR_getInt( ind2 , CHAR_FLOOR ) ){
        return FALSE;
    }
    
    return NPC_Util_Nearby( CHAR_getInt( ind1,CHAR_X),
                            CHAR_getInt( ind1,CHAR_Y),
                            CHAR_getInt( ind2,CHAR_X),
                            CHAR_getInt( ind2,CHAR_Y));
                            
}

/*
 * YES��NO����Ƚ�ꤹ�롣
 *
 * "�Ϥ�" "������" "yes" "no" "��" "��"��ȿ�����롣
 *  "�٣ţ�" "�٣��" "�����" "Yes" "YES" "yes"
 *
 *
 * �֤��͡�  0 : no 1 : yes -1 : �ɤ���Ǥ�ʤ�
 */
static struct ynset
{
    int value;
    char string[32];
}yntable[] ={
    { 0 , "no" },
    { 0 , "No" },
    { 0 , "NO" },
    { 0 , "�Σ�" },
    { 0 , "�Σ�" },
    { 0 , "���" },
    { 0 , "������" },
    { 0 , "������" },

    { 1 , "yes" },
    { 1 , "Yes" },
    { 1 , "YES" },
    { 1 , "�٣ţ�" },
    { 1 , "�٣��" },
    { 1 , "�����" },
    { 1 , "�Ϥ�" },
    { 1 , "�ϥ�" },
    
};

int NPC_Util_YN(char *input )
{
    int i;
    for(i=0;;i++){
        int j;
        
        if( input[i] == '\0' ){
            return -1;
        }
        for(j=0;j<arraysizeof(yntable);j++){
            if( input[i] == yntable[j].string[0] &&
                strncmp( input+i, yntable[j].string,
                         strlen(yntable[j].string))==0){
                return yntable[j].value;
            }
        }
    }
    return -1;
}


/*============================================================
  �������鲼�Τϡ�char.c�����ư������
  ============================================================*/
/*------------------------------------------------------------
 * 2���� point ��Ϳ���ơ����Υݥ���ȴ֤�ľ������������
 * �ʤʤ�������ͥ��Ū�ˤߤ�Τǡ���Ŭ�ǤϤʤ���
 * �����ʤ���꤫����
 * ����
 *  pstart  POINT*  ����
 *  pend    POINT*  ����
 * �֤���
 *  -1  Ʊ������
 *  ����
 ------------------------------------------------------------*/
int NPC_Util_getDirFromTwoPoint( POINT* pstart, POINT* pend )
{
    /*  3x3 ��ɽ�����  */
    /*  y*x */
    static int     dirtable[3][3]={
        {7, 0, 1},
        {6,-1, 2},
        {5, 4, 3},
    };

    int difx, dify;
    difx = pend->x - pstart->x;
    dify = pend->y - pstart->y;

    /*  ����������  */
    if( difx < 0 )difx=-1;
    else if( difx > 0 )difx=1;
    if( dify < 0 )dify=-1;
    else if( dify > 0 )dify=1;

    return dirtable[dify+1][difx+1];
}

/*------------------------------------------------------------
 * ����餬�����ꤷ��ID���ĥ����ƥ�򤤤��Ļ��äƤ��뤫��Ĵ�٤롣
 * ����ϥɥ�NPC�������äƤ��뤫�ɤ�����Ĵ�٤뤿���ɬ�פʴؿ�
 * �Ǥ��롣
 * ����
 *  meindex     int     ������index
 *  itemid      int     �����ƥ��ID itemset.txt�˽񤫤�Ƥ�[ID]
 * �֤���
 *  ���Υ����ƥ���äƤ���Ŀ�
 *  ��ϥ��顼
 ------------------------------------------------------------*/
int NPC_Util_countHaveItem( int meindex , int itemid )
{
    int i, count = 0;
    int maxitemnum;

    if( !CHAR_CHECKINDEX(meindex))return -1;
    maxitemnum = min(CHAR_MAXITEMHAVE,
                     CHAR_getInt(meindex,CHAR_ITEMLIMIT));
    for( i = 0 ; i < maxitemnum ; i ++ ){
        int iind = CHAR_getItemIndex( meindex , i );
        if( ITEM_CHECKINDEX(iind)){
            int id = ITEM_getInt( iind , ITEM_ID );
            if( id == itemid ) count++;
        }
    }
    return count;
}


/*
 * ����饯��������äƤ��륢���ƥ��1�����ä��롣���Υ����ƥ��
 * �����ƥ�ID�ǻ��ꤹ�롣�����ƥ�ɽ�������鸡�����Ƥ��äơ�
 * �ǽ�˥ҥåȤ�����Ĥ������롣
 * 
 * int charindex : ����饯����index
 * int itemid : �����ƥ��ID . itemset.txt �˵��Ҥ���Ƥ��롣
 * BOOL net:    �ͥåȥ���򤹤뤫�ɤ�����TRUE���Ȥ��롣
 *
 * �����ƥ���äƤ��ƺ���Ǥ����� TRUE , ����Ǥ��ʤ��ä���FALSE
 */
BOOL NPC_Util_RemoveItemByID( int charindex, int itemid, BOOL net )
{
    int i;

    for(i=0;i<CHAR_MAXITEMHAVE;i++){
        int ii = CHAR_getItemIndex( charindex , i);
        if( ITEM_CHECKINDEX(ii)
            && ITEM_getInt( ii, ITEM_ID ) == itemid){
            /* ����饯������äƤ��륢���ƥ��*/
            CHAR_setItemIndex( charindex , i , -1 );
            /*
             * ���Υ����ƥ����������õ�롣
             * �Ĥޤꥢ���ƥ�ɽ�������ä���
             */
            ITEM_endExistItemsOne( ii );
            /*  ξ��������    */
            if( ii == CHAR_ARM1 && CHAR_getItemIndex(charindex,CHAR_ARM2)
                == -2 )
                CHAR_setItemIndex(charindex,CHAR_ARM2,-1);

            if(net)CHAR_sendStatusString( charindex, "I");
            return TRUE;
        }
    }
    return FALSE;
}





/*------------------------------------------------------------
 * �դ��ĤΥ���饯������ꤷ�ơ��ҤȤĤΥ���饯�����⤦�ҤȤ�
 * �Υ���饯���ΤԤä�����ˤ��뤫�ɤ�����Ƚ�ꤹ�롣
 * �����⡢����������Υ���������˸����Ƥ��뤫�ɤ�����Ĵ�٤롣
 * ����
 *  frontindex      int     ���ˤ���Ȥ���륭���
 *  backindex       int     ���ˤ���Ȥ���륭��顣
 * �֤���
 *  backindex �� frontindex �Τ�����ˤϤ�Ĥ��Ƥ��顢TRUE��
 *  �������������Ǥʤ����FALSE�򤫤�����
 *
 * ���δؿ��ϡ��ɥ�NPC�ʤɤ�Ƚ��˻Ȥ��롣
 * by ringo
 ------------------------------------------------------------*/
BOOL NPC_Util_isBackContact( int frontindex , int backindex )
{
    /* �����ϡ�4�򤿤���8�� %����ȸ��ˤʤ�Τ� */
    int dir;
    int x,y;
    
    dir = (CHAR_getInt( frontindex , CHAR_DIR ) + 4 ) % 8;
    VALIDATEDIR(dir);
    CHAR_getCoordinationDir( dir ,
                             CHAR_getInt( frontindex , CHAR_X ),
                             CHAR_getInt( frontindex , CHAR_Y ),
                             1 ,
                             &x , &y );
    if( x == CHAR_getInt( backindex , CHAR_X )
        && y == CHAR_getInt( backindex , CHAR_Y )
        && CHAR_getInt( backindex , CHAR_DIR )
        == CHAR_getInt( frontindex , CHAR_DIR ) )
        return TRUE;
    else
        return FALSE;
}

/*
 * �դ��ĤΥ���饯����ľ����˸��������äƤ��ơ�����
 * ��Υ�����륰��åɰ��⤫�ɤ�����Ĵ�٤롣Ź�Ѥ˺�ä���,
 * �फ�����ä���졼�������Ǥ�Ũ�Ȥ��ˤ�Ȥ��롣
 *
 * ����
 * int index1, index2 : ����٤�դ���Υ�����index��
 * int distance : �դ���Τ������ε�Υ
 * �֤���
 * �फ�����äƤ��Ƶ�Υ��distance������ä���TRUE, ����ʳ���FALSE
 * �ŤʤäƤ��Ƥ�FALSE
 */
BOOL NPC_Util_isFaceToFace( int index1, int index2, int distance )
{
    int i;
    if(CHAR_getInt(index1,CHAR_FLOOR) != CHAR_getInt(index2,CHAR_FLOOR)){
        /* �ե�����ä������곰��FALSE */
        return FALSE;
    }
    if(CHAR_getInt(index1,CHAR_X) == CHAR_getInt(index2,CHAR_X )
       && CHAR_getInt(index1,CHAR_Y) == CHAR_getInt(index2,CHAR_Y) ){
        /* �����ʤäƤ�����FALSE */
        return FALSE;
    }
    
    for( i=1;i<=distance;i++ ){
        /* �ޤ������ä���Υ���餬�⤤����κ�ɸ����� */
        int walked_x, walked_y;
        CHAR_getCoordinationDir( CHAR_getInt(index1,CHAR_DIR)
                                 , CHAR_getInt(index1,CHAR_X)
                                 , CHAR_getInt(index1,CHAR_Y)
                                 ,i, &walked_x, &walked_y );
        if( CHAR_getInt(index2,CHAR_X) == walked_x
            && CHAR_getInt(index2,CHAR_Y) == walked_y ){
            /* �⤦���ä��Υ����ȽŤʤä��顢�⤦���ä���Ʊ��
             ����⤫������ɸ����롣�����ƽŤʤä���TRUE��
             �Ťʤ�ʤ��ä���FALSE*/
            CHAR_getCoordinationDir( CHAR_getInt(index2,CHAR_DIR)
                                     , CHAR_getInt(index2,CHAR_X)
                                     , CHAR_getInt(index2,CHAR_Y)
                                     ,i, &walked_x, &walked_y );
            if( CHAR_getInt(index1,CHAR_X) == walked_x
                && CHAR_getInt(index1,CHAR_Y) == walked_y ){
                return TRUE;
            }else{
                return FALSE;
            }
        }
    }
    /* ���󤼤�Ťʤ�ʤ��ä���FALSE */
    return FALSE;
}


/*
 * ���륭��餬���륭���ο����̤β��⤫����ˤ��뤫
 * �ɤ���Ĵ�٤�
 *
 * ����
 * int index1,index2:index1�Υ���������index2�����뤫
 * �ɤ�����Ĵ�٤롣index1�ϸ������ط����뤱�ɡ�index2�ϴط��ʤ���
 * �֤���
 * ��Υ��distance������ä���TRUE, ����ʳ���FALSE
 * �ŤʤäƤ��Ƥ�FALSE
 */
BOOL NPC_Util_charIsInFrontOfChar( int index1, int index2, int distance )
{
    int i;
    if(CHAR_getInt(index1,CHAR_FLOOR) != CHAR_getInt(index2,CHAR_FLOOR)){
        /* �ե�����ä������곰��FALSE */
        return FALSE;
    }
    if(CHAR_getInt(index1,CHAR_X) == CHAR_getInt(index2,CHAR_X )
       && CHAR_getInt(index1,CHAR_Y) == CHAR_getInt(index2,CHAR_Y) ){
        /* �����ʤäƤ�����FALSE */
        return FALSE;
    }
    
    for( i=1;i<=distance;i++ ){
        /* index1�������⤤����κ�ɸ���Ȥ�Ƥ��� */
        int walked_x, walked_y;
        CHAR_getCoordinationDir( CHAR_getInt(index1,CHAR_DIR)
                                 , CHAR_getInt(index1,CHAR_X)
                                 , CHAR_getInt(index1,CHAR_Y)
                                 ,i, &walked_x, &walked_y );
        if( CHAR_getInt(index2,CHAR_X) == walked_x
            && CHAR_getInt(index2,CHAR_Y) == walked_y ){
            /* index2�ȽŤʤä���return TRUE */
            return TRUE;
        }
    }
    /* ���󤼤�Ťʤ�ʤ��ä���FALSE */
    return FALSE;
    
}




/*
 * �ե�����ꤷ�ơ����Υե��ˤ��륭���������Ʊ����å�����
 * ����������ؿ������̤Ȥ��˻Ȥ��Τ�
 *
 *
 *
 *
 */
void NPC_Util_AnnounceFloor( int floorid , char *msg )
{
    int i;
    int charnum = CHAR_getCharNum();
    
    for(i=0;i<charnum;i++){
        if( CHAR_getCharUse(i) &&
            CHAR_getInt( i , CHAR_WHICHTYPE)  == CHAR_TYPEPLAYER  &&
            CHAR_getInt( i , CHAR_FLOOR ) == floorid  ){
            CHAR_talkToCli( i  , -1 , msg , CHAR_COLORWHITE );
        }
    }
}


/*
 * ��¸�Υ����ƥ�򤢤륭���˻������롣
 * �����Τ����äѤ����ɤ������������Ƚ�ꡢ����
 * ���δؿ�����Ǥ�롣
 * 
 * int charindex:�⤿���륭���Υ����index
 * int itemindex:�⤿���륢���ƥ�Υ����ƥ�index
 * BOOL net:    ���δؿ��˥ͥåȥ����餻�� TRUE�ˤ������
 * �֤���:̵���������줿��TRUE,�ʤ�餫�θ����ǻ�������ʤ��ä���FALSE
 *  
 * by nakamura
 *
 *  ���δؿ�����ǡ�����餬���äƤ����Τ��ä��������Τ��鳰����
 * ���̤��֤��Ƥ����Τ��ä����֤��Ƥʤ����롢�Ȥ��������⤷�Ƥ��ޤ�
 * �褦�ˤ�����
 * by ringo
 *
 * some bug fix.( ITEM_setWorkInt etc.)
 *  by taka
 *
 * ���ߤλ��͡�
 *  o   itemindex �Υ����ƥब��ʬ�λ��äƤ��륢���ƥ�Ǥ��ä��顢
 *      return FALSE ����
 */
BOOL NPC_Util_moveItemToChar( int charindex, int itemindex,BOOL net )
{
    int emptyindex, oind ,cind;
    emptyindex = CHAR_findEmptyItemBox( charindex );
    if(  emptyindex < 0 )
        /*  �����ƥ���˶������ʤ�  */
        return FALSE;

    if( !ITEM_CHECKINDEX( itemindex ) )return FALSE;
        
    /* ������������Ϥ���   */
    /*
     * �����Ϸ�������Ƥ��륢���ƥफ������Ȥ⡢
     * ï�������äƤ��륢���ƥफ��Ĵ�٤�
     */
    oind = ITEM_getWorkInt( itemindex , ITEM_WORKOBJINDEX );
    cind = ITEM_getWorkInt( itemindex , ITEM_WORKCHARAINDEX );

    if( oind >= 0 ){
        /* �Ϸ����֤���Ƥ����Τ��� */
        /* �Ϸ��������� */
        CHAR_ObjectDelete( oind );

        /*  �����ƥ���ư  */
        CHAR_setItemIndex( charindex, emptyindex, itemindex );

        /*  �����ƥ��Work�ΰ��񤭤�����  */
        ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1 );
        ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charindex );

        /*  charindex ���ץ쥤�䡼���ä��饢���ƥ��ѹ����ơ���������    */
        if( net )            CHAR_sendStatusString( charindex, "I");

        return TRUE;

    }else if( cind >= 0 ){
        /* ����饯�������äƤ����Τ��� */
        int itemindexinchara;

        /*  ��ʬ�����äƤ�����ˤϲ��⤷�ʤ�  */
        if( cind == charindex )return FALSE;

        /* ��äƤ��Τ�ä� */
        itemindexinchara = NPC_Util_SearchItemInChar( cind , itemindex);
        if( itemindexinchara == -1 )return FALSE;

        CHAR_setItemIndex( cind , itemindexinchara , -1 );
        if( itemindexinchara == CHAR_ARM1
            && CHAR_getItemIndex( cind,CHAR_ARM2 )== -2){
            CHAR_setItemIndex(cind,CHAR_ARM2,-1);
        }
        CHAR_complianceParameter(cind);
        if( net ) CHAR_sendStatusString( cind,"I");
            
        /* �����ƥ��������� */
        CHAR_setItemIndex( charindex , emptyindex , itemindex );
        if( net )  CHAR_sendStatusString( charindex,"I");

        /*  �����ƥ��Work�ΰ��񤭤�����  */
        ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1 );
        ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charindex );

        return TRUE; /* fixed by nakamura(�ʤ��return���Ƥʤ��ä�) */
            
    }
    return FALSE;
}
/*
 * �Ϸ��ˤ�����Ƥ���⤷���ϥ���餬��äƤ��륢���ƥ� 
 * ����Ū���Ϸ��Τ����������֤���
 *
 * int itemindex : �����ƥ�ɽ�ؤ� index
 * int fl,x,y : ���֡�
 * BOOL net: �ͥåȥ���ν����򤹤뤫�ɤ���
 * �֤��ͤ� FALSE : ���� TRUE : ����
 */
BOOL NPC_Util_moveItemToMap( int itemindex , int fl , int x , int y,
                             BOOL net )
{
    int oind,cind;
    
    if( !ITEM_CHECKINDEX( itemindex ) ) return FALSE;

    oind = ITEM_getWorkInt( itemindex , ITEM_WORKOBJINDEX );
    cind = ITEM_getWorkInt( itemindex , ITEM_WORKCHARAINDEX );

    if( oind >= 0 ){
        /* �Ϸ��ˤ�����Ƥ������ñ�˰��֤��ѹ� */
        return MAP_objmove( oind , OBJECT_getFloor(oind),
                     OBJECT_getX(oind),
                     OBJECT_getY(oind),
                     fl,x,y);
    } else if( cind >= 0 ){
        /* ����餬��äƤ��� */
        int itemindexinchara = NPC_Util_SearchItemInChar(cind,itemindex);
        if( itemindexinchara == -1 ) return FALSE;

        /* ����餬��äƤ��ʤ��褦�ˤ��� */
        CHAR_setItemIndex( cind , itemindexinchara , -1 );
        if( itemindexinchara == CHAR_ARM1
            && CHAR_getItemIndex( cind,CHAR_ARM2 )== -2){
            CHAR_setItemIndex(cind,CHAR_ARM2,-1);
        }
        CHAR_complianceParameter(cind);
        if( net ) CHAR_sendStatusString( cind,"I");

        /* �����ƥ�index����ľ�ܵ��� */
        if( CHAR_DropItemAbsolute( itemindex, fl,x,y,TRUE) < 0 ){
            return FALSE;
        } else {
            return TRUE;
        }
    }
    return FALSE;
}


/*
 * ����餬��äƤ��륢���ƥ�������ۤ��Υ����˰ܹԤ��롣
 * ���ΤȤ��˰ܹԤ�����ʤ�ʬ�ϤΤ���
 *
 * int give : �錄�����
 * int take : �錄����
 * return :
 *  �錄����������
 *  ��ϥ��顼
 */
int NPC_Util_GiveAllItemToChar( int give , int take )
{
    int i ;
    int count=0;
    
    for(i=0;i<CHAR_MAXITEMHAVE;i++){
        int itemindex = CHAR_getItemIndex( give , i );
        BOOL aho = NPC_Util_moveItemToChar( take , itemindex,FALSE );
        if( aho ){
            count++;
        }
    }
    if( count ){
        /*  �ͥåȥ���򤹤�  */
        CHAR_sendStatusString( give,"I");
        CHAR_sendStatusString( take,"I");
    }
    return count;
}
 
/*
 * �����ƥ���������ƥ����˻������롣
 * �����Τ����äѤ����ɤ������������Ƚ�ꡢ����
 * ���δؿ�����Ǥ�롣
 * int charindex:�⤿���륭���Υ����index
 * int itemid:itemset.txt�����ꤵ��Ƥ��륢���ƥ�id
 * BOOL net �ͥåȥ���򤹤뤫�ɤ���
 *
 * �֤���:̵���������줿��TRUE,�ʤ�餫�θ����ǻ�������ʤ��ä���FALSE
 *
 * by nakamura
 */
BOOL NPC_Util_createItemToChar( int charindex, int itemid , BOOL net)
{
    int emptyitemindexinchara, itemindex;
    emptyitemindexinchara = CHAR_findEmptyItemBox( charindex );

    if( emptyitemindexinchara < 0 )return FALSE;

	itemindex = ITEM_makeItemAndRegist( itemid );

    if( itemindex != -1 ){
        CHAR_setItemIndex( charindex, emptyitemindexinchara, itemindex );
        /*  Work�ǡ���������    */
        ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
        ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charindex);

        if(net)CHAR_sendStatusString(charindex,"I");
        return TRUE;
    }
    return FALSE;
}

/*------------------------------------------------------------
 * ���饹����ǡ������ƥ���롣
 * ����
 *  charaindex  int     ����饤��ǥå���
 *  class       int     ���饹
 *  net         BOOL    TRUE ���ȥͥåȥ������
 * �֤���
 *  1       ����
 *  -1      ���顼
 *  -2      �����Υ����ƥ��󤬤��äѤ�
 ------------------------------------------------------------*/
BOOL NPC_Util_makeItemFromClass( int charaindex, int class, BOOL net )
{
    int     itemid;
    int     emptyindex;
    int     itemindex;

    itemid = ITEM_getItemIdFromClass( class );
    if( itemid == -1 )return -2;

    emptyindex = CHAR_findEmptyItemBox( charaindex );
    if(  emptyindex < 0 )return -1;

    itemindex = ITEM_makeItemAndRegist( itemid );
    if( itemindex == -1 )return -2;

    CHAR_setItemIndex(charaindex,emptyindex,itemindex);

    ITEM_setWorkInt(itemindex,ITEM_WORKOBJINDEX,-1);
    ITEM_setWorkInt(itemindex,ITEM_WORKCHARAINDEX,charaindex);

    if( net )CHAR_sendStatusString( charaindex,"I");
    return TRUE;
}


/*
 * �դ��ĤΥ����Τ������ε�Υ����롣
 * �����Ǥε�Υ�Ȥϡ�2�Ԥκ�ɸ�κ���������
 * ���礭���ۤ���
 * �ޤ���floor���㤦���Ϥ������礭���ͤ��֤���
 * �������뤳�Ȥ�floor�ΰ㤤���̥롼�����
 * ���ޤ�ռ����ʤ����ɤ��ʤ뤬���ޤ��ɤ������Ǥ�
 * ̵�����⤷��ʤ������줫���ɤ��ơ�
 *
 * int index1, index2:��Υ���᤿��������index
 * �֤���:��Υ���Τ��
 *
 * by nakamura
 */
int NPC_Util_CharDistance( int index1, int index2 )
{
    int dx, dy;

    /* floor�����������ϡ����夦�֤�˱󤤤Ȥ�����̣��
       int�κ����ͤ�0x7fffffff���֤���*/ 

    if( CHAR_getInt(index1,CHAR_FLOOR)!= CHAR_getInt(index2,CHAR_FLOOR) ){
        return 0x7fffffff;
    }
    
    dx  =ABS(  CHAR_getInt(index1,CHAR_X)- CHAR_getInt(index2,CHAR_X) );
    dy  =ABS(  CHAR_getInt(index1,CHAR_Y)- CHAR_getInt(index2,CHAR_Y) );

    return max( dx, dy );

}
/*------------------------------------------------------------
 * ���ֶᤤ�����θ���
 *
 * �ץ쥤�䡼������������ǥץ쥤�䡼��Ũ���򸡺�����褦�ˤ���
 *  NPC_Util_SearchNearPlayer ����NPC_Util_SearchNear���ѹ�
 *  NPC_Util_SearchNearPlayer ��NPC_Util_SearchNear��ƤӽФ���
 *  ����ΤǺ��ޤǤȥ��󥿡��ե��������Ѥ�äƤ��ʤ����ʲ��ġ�
 *
 *	������	int meindex ��ʬ��index
 *			int maxlen	�����ϰϡ� ����åɿ� ��
 *          int type    CHAR_TYPEPLAYER, CHAR_TYPEENEMY
 *
 *	�֤��͡�
 *      ���ֶᤤŨ��index
 *      ���顼�λ��� -1
 ------------------------------------------------------------*/
static int  SearchNearLine( /* SearchNearAround ����Υ��� */
	int xStart, /* ��������غ�ɸ�κǽ� */
	int yStart, /* ��������ٺ�ɸ�κǽ� */
	int floor,  /* ��������ե� */
	int xPlus,  /* ����ʬ */
	int yPlus,  /* ����ʬ */
	int Count,  /* ���󸡺����뤫 */
	int type    /* �������� CHAR ������ */
){
	OBJECT object;
	int i, x = xStart, y = yStart;

	for( i = 0; i < Count; i ++ ){
		for( object=MAP_getTopObj( floor, x,y ); object ; 
			object = NEXT_OBJECT(object) )
		{
			int objindex = GET_OBJINDEX(object), iWork;
			/* Ʊ����ɸ�˥���饯�������뤫 */
    		if( OBJECT_getType(objindex) != OBJTYPE_CHARA )continue;
    		iWork = OBJECT_getIndex(objindex);
    		/* �ҥåȥݥ���ȥޥ��ʥ���̵�� */
    		if( CHAR_getInt(iWork,CHAR_HP) < 0 )continue;
			/* ���Υ���饯�����ߤ��������פ��� */
			if( CHAR_getInt( iWork,CHAR_WHICHTYPE ) == type ){
			/* �Ĥ���ȯ�� */
				return iWork;
			}
		}
		/* ���Υե�����ɤ򤵤� */
		x += xPlus;	y += yPlus;
	}
	return -1;
}


static int SearchNearAround( 
	int x, 	   /* �����濴�Σغ�ɸ */
	int y,     /* �����濴�Σٺ�ɸ */
	int floor, /* �����濴�Υե� */
	int Part,  /* �������Ϥ���ǽ�Υ饤�� */
	int Level, /* �濴���Υ��Ƥ����Υ */
	int type   /* �������� CHAR ������ */
){
	int i, iTarget = -1;
	for( i = 0; i < 4; i ++ ){
		if( Part == 0 ){ /* ��Υ饤��򸡺� */
			iTarget = SearchNearLine( x-Level+1, y-Level, 
				floor, 1, 0, Level*2, type );
			if( iTarget >= 0 )break;
		}else
		if( Part == 1 ){ /* ���Υ饤��򸡺� */
			iTarget = SearchNearLine( x+Level, y-Level+1, 
				floor, 0, 1, Level*2, type );
			if( iTarget >= 0 )break;
		}else
		if( Part == 2 ){ /* ���Υ饤��򸡺� */
			iTarget = SearchNearLine( x+Level, y+Level, 
				floor, -1, 0, Level*2, type );
			if( iTarget >= 0 ) break;
		}else
		if( Part == 3 ){ /* ���Υ饤��򸡺� */
			iTarget = SearchNearLine( x-Level, y+Level, 
				floor, 0, -1, Level*2, type );
			if( iTarget >= 0 ) break;
		}
		Part ++; Part &= 3; /* ���Υѡ��Ȥ򸡺� */
	}
	return iTarget;
}

int NPC_Util_SearchNear( int meindex, int maxlen, int type )
{
#if 1
	/* MAP ����Υ�󥯤Ǹ�������褦���ѹ� */
    int	floor, x, y, i,iTarget = -1;
	/* ���ߤμ�ʬ�ξ��������Ф� */
    if( !CHAR_CHECKINDEX( meindex ) )return -1;
    floor = CHAR_getInt( meindex, CHAR_FLOOR );
    x  = CHAR_getInt( meindex, CHAR_X );
    y  = CHAR_getInt( meindex, CHAR_Y );

	/* �����ϰ����Ĵ�٤� */
	for( i = 0; i < maxlen; i ++ ){
		iTarget = SearchNearAround( x, y, floor, RAND( 0,3 ), i+1, type );
		if( iTarget >= 0 )break;
	}
	return iTarget;


#else
	/* ������ϸŤ������ */
    int	floor, x, y, i,iTarget = -1, iLen, iMin = 655360, tX, tY;
    /* �ѿ����å�*/
    if( !CHAR_CHECKINDEX( meindex ) )return -1;
    floor = CHAR_getInt( meindex, CHAR_FLOOR );
    x  = CHAR_getInt( meindex, CHAR_X );
    y  = CHAR_getInt( meindex, CHAR_Y );

    /*  ���� */
    for( i = 0 ; i<objnum ; i++ ){
        /*����饯�����λ�*/
        if( obj[i].type != OBJTYPE_CHARA ) continue;
		/* ���̤򸫤�*/
        if( CHAR_getInt( obj[i].index,CHAR_WHICHTYPE)
            != type ) continue;
		/* Ʊ���ե��λ�*/
	    if( floor != obj[i].floor ) continue; 
	    
	    /* CHAR_TYPEENEMY �λ���index����ʬ�ξ�礬����Τǥ����å����� */
        if( obj[i].index == meindex ) continue;

		/* �أ٤ε�Υ��׻�*/
		tX = ABS( x-CHAR_getInt(obj[i].index,CHAR_X) );
		tY = ABS( y-CHAR_getInt(obj[i].index,CHAR_Y) );
		if( tX > maxlen )continue;	/* Υ�줹��*/
		if( tY > maxlen )continue;	/* �ʤ鼡��*/

        /* �٤�����Υ��׻�*/
		iLen = tX*tX+tY*tY;
		/* ���ֶᤫ�ä����ϵ�Ͽ����*/
		if( iMin > iLen ){
			iMin = iLen;
			iTarget = obj[i].index;	/* ����ǥå�������Ͽ*/
		}
	}
	
	return iTarget;
#endif
}
/*------------------------------------------------------------
* ���ֶᤤ�ץ쥤�䡼�θ���
------------------------------------------------------------*/
int NPC_Util_SearchNearPlayer( int meindex, int maxlen )
{
	return( NPC_Util_SearchNear( meindex, maxlen, CHAR_TYPEPLAYER));
}
/*------------------------------------------------------------
* ���ֶᤤŨ�θ���-
-----------------------------------------------------------*/
int NPC_Util_SearchNearEnemy( int meindex, int maxlen )
{
	return( NPC_Util_SearchNear( meindex, maxlen, CHAR_TYPEENEMY));
}



/*******************************************************

  �Ф��ư�������˲����ˤ֤Ĥ��äƤ��뤫�ɤ���Ĵ�٤�
  int NPC_Util_SuberiWalk( 
	int	index,	CHAR �Υ���ǥå���
	int	dir		�⤯����
  );

  �����  0��7���⤱������
          -1  ���ޥåפ˰��ä����äƥ��顼
          -2  ������饯���˰��ä����äƥ��顼
          -3  ��ξ���˰��ä����äƥ��顼

  �Ф��ư������ޤ��ġ����˰�ư�Ǥ��뤫Ĵ�٤롣
  ξ����ư�Ǥ���мФ������å����롣����������ư�Ǥ��ʤ����
  ��������������֤���

 �����˲������ä���ʥޥåװʳ��ˣ����֤�
*******************************************************/ 
int NPC_Util_OtherCharCheck( int ff, int fx, int fy)
{
#if 1
	/* ����������ͣ��Ф���Υ�󥯤��ѹ� */
	OBJECT object;
	for( object=MAP_getTopObj(ff,fx,fy); object ; 
		object = NEXT_OBJECT(object) )
	{
		int objindex = GET_OBJINDEX(object);
		/* Ʊ����ɸ�˥���饯�������뤫 */
    	if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
    		return 1;
		}
	}
	return 0;
#else
	/* ������ϵ켰�Τ���� */
    int i;
    for( i = 0 ; i < objnum ; i ++){
		/* ��ɸ��Ʊ������*/
        if( obj[i].x == fx && obj[i].y == fy && obj[i].floor == ff ){
        	/* ����饯������*/
	        if( obj[i].type != OBJTYPE_NOUSE ){
	            return 1;
            }
        }
    }
	return 0;
#endif
}


/* �⤱�ʤ����ϳ�äƤߤ�*/
int NPC_Util_SuberiWalk( 
	int	index,
	int	dir
)
{
	int	i, OkFlg = 0, x = 0, y = 0, fl = 0;
	int aDirList[3],dir2;
	int tX, tY;

    x  = CHAR_getInt(index,CHAR_X);
    y  = CHAR_getInt(index,CHAR_Y);
    fl = CHAR_getInt(index,CHAR_FLOOR);

	/* �Ф���������*/
	switch( dir ){
	case 1: aDirList[0] = 0; aDirList[1] = 2; aDirList[2] = 1;
		break;
	case 3: aDirList[0] = 2; aDirList[1] = 4; aDirList[2] = 3;
		break;
	case 5: aDirList[0] = 4; aDirList[1] = 6; aDirList[2] = 5;
		break;
	case 7:	aDirList[0] = 6; aDirList[1] = 0; aDirList[2] = 7;
		break;
	default:
		tX = x+CHAR_getDX(dir);
		tY = y+CHAR_getDY(dir);
		/* �ʤ᤿*/
		if( MAP_walkAble( index,fl,tX,tY ) == 0 ){
			dir = -1;/* �ޥåפ˰��ä����ä�*/
		}else
		if( NPC_Util_OtherCharCheck( fl,tX, tY ) ){
			dir = -2;/* �����˰��ä����ä�*/
		}
		/* �ޤä�����!! ���Τޤ��⤯*/
/*		CHAR_walk( index, dir, 0 );*/
		return dir;
	}

	/* �ޤä����ʤ�Ƚ��ʤ�*/
	for( i = 0; i < 3; i ++ ){
		dir2 = aDirList[i];
		tX = x+CHAR_getDX(dir2);
		tY = y+CHAR_getDY(dir2);
		if( MAP_walkAble( index,fl,tX,tY ) == 0 ){
			aDirList[i] = -1; /* �Ԥ��ʤ����Ϥ�����������*/
		}else if( NPC_Util_OtherCharCheck( fl,tX, tY ) ){
			aDirList[i] = -2; /* �Ԥ��ʤ����Ϥ�����������*/
		}else{
			/* �Ԥ���!!*/
			OkFlg ++;
		}
	}

	/* ����������ǽ�ʾ��ϡ��ǽ���̤�ư��*/
	if( aDirList[0] >= 0 && aDirList[1] >= 0 && aDirList[2] >= 0 ){
	}else
        /* �������ư���뤫��*/
	if( aDirList[0] >= 0 ){
		dir = aDirList[0];
	}else
        /* ����Ȥ⤳���餫��*/
	if( aDirList[1] >= 0 ){
		dir = aDirList[1];
	}else{
		dir = aDirList[0]+aDirList[1];
	}

	/* ��ư*/
/*	CHAR_walk( index, dir, 0 );*/

	return dir;
} 
/*-----------------------------------------------------------------------
* NPC��create���ΰ������椫����ꤵ�줿ʸ����򸡺�����
* ������ο��ͤ�������롣���ץ����ǿ��ͤ���ꤵ���Ƥ�ꤿ���Ȥ�������
* 
* ��ա��ǥ�ߥ���"|"�˸��ꤷ�Ƥޤ���
*       ������Ʊ��ʸ���󤬣����¤����硤
*       �������ͥ�褵��ޤ���
* ----------------------------------
* IN : int meindex : �����index
*    : char* in    : ��������ʸ����
* OUT:
* RETURN: 
* ��������ʤ��ä��� :-1
* �������褿��       :����
* ----------------------------------
*
* �㡧
* [File:enemykill.create]
* enemy=enemykill|range:4
*
* call:
* range = NPC_Util_GetNumFromArg( index, "range");
*----------------------------------------------------------------------*/
int NPC_Util_GetNumFromArg( int meindex, char* in)
{
	BOOL    rc;
	int     i;
	char    outstr[sizeof( STRING32)];
	int		out = -1;
	char 	*a = CHAR_getChar( meindex, CHAR_NPCARGUMENT);
	
	for( i = 1; ; i++ ) {
		/* �ǥ�ߥ����Ĥ����ɤ� */
		rc = getStringFromIndexWithDelim( a, "|", i, outstr, sizeof(outstr));
		if( rc == FALSE) break;
		if( strstr( outstr, in) != NULL ) {
			out = textToInt( outstr);
			break;
		}
	}
	
	return out;
}

/*
 * �����ƥ�� index �ȥ���� index �Τդ��Ĥξ��󤫤顢
 * ����餬���Υ����ƥ����äƤ뤫�ɤ�������٤ƻ��äƤ���ɤ�
 * ���֤˻��äƤ뤫���֤���  �����ƥ��Ϣ��NPC�ѡ�CHAR_�˰ܹ�
 * �Ǥ��뤫�⤷���ؿ��ʥ�
 * by ringo
 *
 * int charindex : �оݥ����� index
 * int itemindex : �����ƥ��index
 *
 * �֤���
 *
 *  >=0 : �ɤΰ��֤˻��äƤ���Τ���ͭ�����͡�
 *  -1 : �⤷���ϻ��äƤʤ���
 *  -2 : ���顼��
 */
int NPC_Util_SearchItemInChar( int charindex , int itemindex)
{
    int i;
    
    if( !CHAR_CHECKINDEX(charindex) ) return -2;
    if( !ITEM_CHECKINDEX(itemindex) ) return -2;

    for(i=0;i<=CHAR_MAXITEMHAVE;i++){
        int ii = CHAR_getItemIndex( charindex , i );
        if( ii == itemindex ){
            return i;
        }
    }
    return -1;
}


/*
 *  ���äƤ��륢���ƥ�������Ѵ����롣�����ƥ�ɽ���������������ƥ��
 *  ���ä��롣
 *  ��Ȥ�Ȥ�SimpleShop�ν���������ѡ�
 *  Ź�ʳ��ˤ⡢�����ƥ�ɽ�򤹤ä��ꤷ�Ƥ�����(�Ȥ�����ɬ�פ�)�Ȥ�
 *  �ˤĤ�����
 *  int charind : ����оݤΥ���饯����
 *
 *  by ringo 
 */
void NPC_Util_ReleaseHaveItemAll( int charind )
{
    int i;
    if( !CHAR_CHECKINDEX( charind ) ) return;
    
    for(i=0;i<CHAR_MAXITEMHAVE ;i++){
        int iind =  CHAR_getItemIndex( charind , i );
        if( iind < 0 ) continue;
        
        /* �����ƥ�ɽ��ä��Ƥ��� */
        ITEM_endExistItemsOne( iind );

        /* ��ʬ�Τ�äƤ���Τ�ä� */
        CHAR_setItemIndex( charind,i ,-1);

        /*  ξ����������θ����  */
        if( i == CHAR_ARM1 && CHAR_getItemIndex(charind,CHAR_ARM2) == -2 )
            CHAR_setItemIndex( charind, CHAR_ARM2, -1);
    }
}
/*------------------------------------------------------------------------
* ����餫��ߤ�¾�Υ��������������
*
* int NPC_Util_GetDirCharToChar( int fromindex, int toindex, int mode)
*
* int	fromindex 	���Υ����index����
* int	toindex		���Υ����index���Ф��Ƥ�������������
* int	mode		������������
*                   0:toindex�˸����ä�
*                   1:toindex��ȿ������
*                   2:toindex���Ф��Ʊ�����������
*                   3:toindex���Ф��ƺ�����������
* 
* �֤��� ���� ��dir���������֤���
*        ���� ��-1
------------------------------------------------------------------------*/
int NPC_Util_GetDirCharToChar( int fromindex, int toindex, int mode)
{
	int	ret = -1;
	
	if( CHAR_getInt( fromindex, CHAR_FLOOR) 
		== CHAR_getInt( toindex, CHAR_FLOOR))
	{
		POINT	start,end;
		int		dir;
		start.x = CHAR_getInt( fromindex, CHAR_X);
		start.y = CHAR_getInt( fromindex, CHAR_Y);
		end.x = CHAR_getInt( toindex, CHAR_X);
		end.y = CHAR_getInt( toindex, CHAR_Y);
		dir = NPC_Util_getDirFromTwoPoint( &start,&end );
		if( mode == 1 )		dir = (dir > 3)? dir -4 : dir +4;
		else if( mode == 2) dir = (dir + 2) % 8;
		else if( mode == 3) dir = (dir < 1)? dir +6 : dir -2;
		ret = dir;
	}
	return( ret);
}

/*------------------------------------------------------------------------
* ��ɸ�������Ф��ơ����������˰�ư���롣
*
* int NPC_Util_WalkCharToChar( int fromindex, int toindex, int mode)
*
* int	fromindex 	�⤯�����index
* int	toindex		�⤯��ɸ�Υ����index
* int	mode		�⤯����
*                   0:toindex�˸����ä�
*                   1:toindex��ȿ������
*                   2:toindex���Ф��Ʊ�����������
*                   3:toindex���Ф��ƺ�����������
* BOOL	suberi		NPC_Util_SuberiWalk���θ���뤫��TRUE:���� FALSE:���ʤ�
* 
* �֤��� �������⤤���ˡ�dir���⤤���������֤���
*        ����          ��-1
------------------------------------------------------------------------*/
int NPC_Util_WalkCharToChar( int fromindex, int toindex, int mode, int suberi)
{
	int	ret = -1;
	int	dir;
	
	if( CHAR_getInt( fromindex, CHAR_FLOOR) 
		!= CHAR_getInt( toindex, CHAR_FLOOR))
	{
		return( -1);
	}
	dir = NPC_Util_GetDirCharToChar( fromindex, toindex, mode );
	if( dir != -1) {
		if( suberi) {
			dir = NPC_Util_SuberiWalk( fromindex, dir );
		}
	}
	if( dir >= 0 ) {
		ret = CHAR_walk(fromindex,dir,0);
		if( ret == CHAR_WALKSUCCESSED ) ret = dir;
		else							ret = -1;
	}
	return( ret);
}

/*
 *  NPC���̿��ѡ�
 *
 *  ¾��NPC�򥳥�ȥ��뤹�����Ū�ʴؿ���
 * ʸ���������˼�ꡢ��䤳�������Ȥ���ˤҤ������롣
 * ���Ȥ��С��ɥ��� NPC_DoorOpen ����äƤ��뤫�Ȥ���
 * �����Ĥ��ΤäƤ��롣
 *
 *
 * CHAR_TYPE chartype : �����μ��ऴ�Ȥ�ʬ�ष�Ƥ���enum
 * char *npcname : NPC�θ���̾
 * char *command : ���ޥ��
 *
 * ����command �ϰʲ���ʸ������б����Ƥ��롣
 *
 * open : NPC_???Open ���ܤ��Ȥ��롣
 * close : NPC_???Close ���ܤ��Ȥ��롣
 * flip : NPC_???flip ���ܤ��Ȥ��롣
 * counter+N : NPC_????AddSwitchCounter���ܤ��Ȥ��롣
 * counter-N : NPC_????AddSwitchCounter���ܤ��Ȥ��롣
 * counter=N : NPC_????SetSwitchCounter ���ܤ��Ȥ��롣
 * kill : NPC_????KillByOtherNPC ���ܤ��Ȥ��롣
 * fillitembyid=ID : NPC_????FillItemByID ���ܤ��Ȥ��롣
 * removeitembyid=ID : NPC_????RemoveItemByID ���ܤ��Ȥ��롣
 *
 * return value:
 *  -1 : �����NPC�Ϥ��ʤ�
 *  -2 : chartype����������
 *  -3 : command ������Ǥ��ʤ���
 *  -4 : ����NPC�Ϥޤ����ݡ��Ȥ��Ƥʤ���
 *   0 : ����
 */
enum
{
    CTRLOTHERNPC_ERROR = -1,
    CTRLOTHERNPC_OPEN = 0 ,
    CTRLOTHERNPC_CLOSE,
    CTRLOTHERNPC_FLIP, 
    CTRLOTHERNPC_COUNTERMOD,
    CTRLOTHERNPC_COUNTERSET,
    CTRLOTHERNPC_KILL,
    CTRLOTHERNPC_FILLITEMBYID,
    CTRLOTHERNPC_REMOVEITEMBYID,
    CTRLOTHERNPC_MAX

};
static int NPC_Util_ControlOtherNPC_getActiontype( char *com,STRING64 *opt);


int NPC_Util_ControlOtherNPC( CHAR_TYPE chartype ,
                              char *npcname,
                              char *command )
{

    STRING64 option;
    int actiontype = NPC_Util_ControlOtherNPC_getActiontype( command ,&option);

    switch( chartype ){
        
    case CHAR_TYPEFIREMAGIC :      
    case CHAR_TYPETHUNDERMAGIC :   
    case CHAR_TYPEICEMAGIC : 
    case CHAR_TYPEOTHERMAGIC :
        return -4;
        
    case CHAR_TYPEDOOR :
    {
        /* �ޤ��ɥ��򸡺� */
        int doorindex = NPC_DoorSearchByName( npcname );
        if( doorindex < 0) return -1;
        switch(actiontype){
        case CTRLOTHERNPC_OPEN:
            NPC_DoorOpen( doorindex , -1 );
            break;
        case CTRLOTHERNPC_CLOSE:
            NPC_DoorClose( doorindex, -1 );
            break;
        case CTRLOTHERNPC_FLIP:
            NPC_DoorFlip( doorindex , -1 );
            break;
        case CTRLOTHERNPC_COUNTERMOD:
            NPC_DoorAddSwitchCounter( doorindex, atoi( option.string), -1 );
            break;
        case CTRLOTHERNPC_COUNTERSET:
            NPC_DoorSetSwitchCounter( doorindex, atoi( option.string),-1 );
            break;
        default:
            return -3;
        }
        return 0;
    }
    
    case CHAR_TYPEBOX :
    case CHAR_TYPESHIP :
    case CHAR_TYPEMSG :
    case CHAR_TYPEWARP :
    case CHAR_TYPESWITCH :
    case CHAR_TYPESHOP :
    case CHAR_TYPEHEALER :
    case CHAR_TYPEOLDMAN :
    case CHAR_TYPEKEYMAN :
    case CHAR_TYPETIMEDOOR :
    case CHAR_TYPEROOMADMIN :
    case CHAR_TYPETOWNPEOPLE :
    case CHAR_TYPEOTHERNPC :
        return -4;
    default:
        return -2;
    }
    return -2;
}



/*
 * ACTION ��enum ���ͤΤɤ줫������
 * 
 * char *com : ���ޥ�����Τ��Ϥ��äƤ�ʸ����
 * STRING64 *opt:  ���ץ�����ʸ����
 * return value:
 * CTRLOTHERNPC_???? ������
 * 
 */
static int NPC_Util_ControlOtherNPC_getActiontype( char *com, STRING64 *opt)
{
    strcpysafe( opt->string , sizeof(opt->string),"" );    
    if( strcmp( "open" , com ) == 0 ){
        return CTRLOTHERNPC_OPEN;
    } else if( strcmp( "close" , com ) == 0 ){
        return CTRLOTHERNPC_CLOSE;
    } else if( strcmp( "flip" , com ) == 0 ){
        return CTRLOTHERNPC_FLIP;
    } else if( strncmp( com , "counter+" , strlen("counter+")) == 0 ){
        strcpysafe( opt->string,sizeof(opt->string),com + strlen("counter+"));
        return CTRLOTHERNPC_COUNTERMOD;
    } else if( strncmp( com , "counter-" , strlen("counter-")) == 0 ){
        strcpysafe( opt->string,sizeof(opt->string),com + strlen("counter-"));        
        return CTRLOTHERNPC_COUNTERMOD;        
    } else if( strncmp( com , "counter=" , strlen("counter=")) == 0 ){
        strcpysafe( opt->string,sizeof(opt->string),com + strlen("counter="));        
        return CTRLOTHERNPC_COUNTERSET;
    } else if( strcmp( com , "kill") == 0 ){
        return CTRLOTHERNPC_KILL;
    } else if( strncmp( com , "fillitembyid=",strlen( "fillitembyid=")) == 0 ){
        strcpysafe( opt->string,sizeof(opt->string),com + strlen("fillitembyid="));        
        return CTRLOTHERNPC_FILLITEMBYID;
    } else if( strncmp( com , "removeitembyid=",strlen( "removeitembyid=")) == 0 ){
        strcpysafe( opt->string,sizeof(opt->string),com + strlen("removeitembyid="));          
        return CTRLOTHERNPC_REMOVEITEMBYID;
    } else {
        return CTRLOTHERNPC_ERROR;
    }
}

/*------------------------------------------------------------------------
* NPC����Ǥ����롣
*
* void NPC_Util_NPCDelete( int srcindex)
*
* int	srcindex 	�ä�index
* 
------------------------------------------------------------------------*/
void NPC_Util_NPCDelete( int srcindex)
{
    int objindex;
    objindex = CHAR_getWorkInt(srcindex,CHAR_WORKOBJINDEX);

    NPC_createDecreaseEnemynum( CHAR_getInt(srcindex,
                                            CHAR_NPCCREATEINDEX ) );

	CHAR_CharaDelete( srcindex );

}
/*------------------------------------------------------------------------
 * create������ArgFile�λ��꤬���뤫Ĵ�٤�
 * static char *NPC_Util_CheckAssignArgFile( int index, char *filename)
 *
 * IN		int		index		�����index
 * OUT		char	*filename	�ե�����̾
 *
 * RETURN	����	filename�Υ��ɥ쥹
 *			����	NULL
 *----------------------------------------------------------------------*/
static char *NPC_Util_CheckAssignArgFile( int index, char *filename)
{
	BOOL    rc;
	int     i;
	char	*cret = NULL;
	char    outstr[64];
	char    outstr2[64];
	char 	*a = CHAR_getChar( index , CHAR_NPCARGUMENT );
	
	for( i = 0; ; i++ ) {
		/* �ǥ�ߥ����Ĥ����ɤ� */
		rc = getStringFromIndexWithDelim( a, "|", i, outstr, sizeof(outstr));
		if( rc == FALSE) break;
		if( strstr( outstr, "file") != NULL ) {
			rc = getStringFromIndexWithDelim( outstr, ":", 2, 
											outstr2, sizeof(outstr2));
			if( rc != FALSE ) {
				strcpy( filename, outstr2);
				cret = filename;
				break;
			}
		}
	}
	return( cret);
}
/*------------------------------------------------------------------------
 * ArgFile����Ȥ�"|"��Ϣ�뤷��ʸ������֤�
 * static char *NPC_Util_MargeStrFromArgFile( char *filename, char *buf)
 *
 * IN		char	*filename	�����ե�����̾
 *			int		len			buf�Υ�����
 * OUT		char	*buf		Ϣ�뤵�줿����
 *
 * RETURN	����	buf�Υ��ɥ쥹
 *			����	NULL
 *----------------------------------------------------------------------*/
static char *NPC_Util_MargeStrFromArgFile( char *filename, char *buf, int len)
{
	FILE	*fp;
	char	*cret = NULL;
	char	line[NPC_UTIL_GETARGSTR_LINEMAX];
	char	opfile[32];
	
	strcpy( opfile, "data/npc/");
	strcat( opfile, filename);
	*buf = '\0';
	fp = fopen( opfile, "r");
	if( fp != NULL ) { 
		while( fgets( line, sizeof( line), fp)) {
			if( strlen(buf) != 0 ) {
				if( buf[strlen(buf) -1] != '|' ) {
					strcatsafe( buf, len, "|");
				}
			}
			chompex( line);
			strcatsafe( buf,len,  line);
		}
		fclose( fp);
	}
	return( cret);
}
/*------------------------------------------------------------------------
 * �����ե����뤫��Ȥΰ�������������������
 * char *NPC_Util_GetArgStr( int index, char *argstr)
 * 
 * IN		int		index		�����index
 *          int		len			argstr�Υ�����
 * OUT		char	argstr		������ʸ����
 *
 * RETURN	����	argstr�Υ��ɥ쥹
 *			����	NULL
 *----------------------------------------------------------------------*/
char *NPC_Util_GetArgStr( int index, char *argstr, int len)
{
	char	filename[32];
	char	*cret = NULL;
	
	/* �ե������Ĵ�٤�*/
	if( NPC_Util_CheckAssignArgFile( index, filename) != NULL ) { 
		/* ���������� */
		if( NPC_Util_MargeStrFromArgFile( filename, argstr, len) != NULL ) {
			cret = argstr;
		}
	}
	/* �ʤ����.create�ե�����ΰ����򤽤Τޤ��֤� */
	else {
		strcpy( argstr, CHAR_getChar( index, CHAR_NPCARGUMENT));
		cret = argstr;
	}
	return( cret);
}

/*-----------------------------------------------------------------------
* �ǥ�ߥ�("|")�Ƕ��ڤ�줿ʸ������椫����ꤵ�줿ʸ����򸡺�����
* ������ο��ͤ�������롣
* 
* ��ա��ǥ�ߥ���"|"�˸��ꤷ�Ƥޤ���
*       ������Ʊ��ʸ���󤬣����¤����硤
*       �������ͥ�褵��ޤ���
* ----------------------------------
* IN : char *srcstr	: ������ʸ����
*      char *in     : ��������ʸ����
* OUT:
* RETURN: 
* ��������ʤ��ä��� :-1
* �������褿��       :����
* ----------------------------------
*----------------------------------------------------------------------*/
int NPC_Util_GetNumFromStrWithDelim( char *srcstr, char* in)
{
	BOOL    rc;
	int     i;
	char    outstr[32];
	int		out = -1;
	
	for( i = 1; ; i++ ) {
		/* �ǥ�ߥ����Ĥ����ɤ� */
		rc = getStringFromIndexWithDelim( srcstr, "|", i, outstr, sizeof(outstr));
		if( rc == FALSE) break;
		if( strstr( outstr, in) != NULL ) {
			out = textToInt( outstr);
			break;
		}
	}
	
	return out;
}
/*-----------------------------------------------------------------------
 * "aaa|bbb:ccc|ddd:eee|fff|ggg"
 * �Ȥ���ʸ������椫��"bbb"����ꤹ���"ccc"���äƤ����ؿ�
 * char *NPC_Util_GetStrFromStrWithDelim( char *srcstr, 
 *										  char *srhstr, 
 *										  char *buf)
 *
 * IN	char *srcstr		������ʸ����
 *		char *srhstr		õ��ʸ����
 *		int	  len			��Ǽ�Хåե��Υ�����
 * OUT	char *buf			��Ǽ����Хåե�
 *
 * RETURN	����			buf�Υ��ɥ쥹
 *			����			NULL
 *---------------------------------------------------------------------*/
char *NPC_Util_GetStrFromStrWithDelim( char *srcstr, char *srhstr, 
                                       char *buf, int buflen)
{
	BOOL    rc;
	char	*cret = NULL;
	int     i;
	char    outstr[1024];
	char    outstr2[1024];
	
	for( i = 1; ; i++ ) {
		/* �ǥ�ߥ����Ĥ����ɤ� */
		rc = getStringFromIndexWithDelim( srcstr, "|", i, outstr,
                                          sizeof(outstr));
		if( rc == FALSE) break;
		if( strstr( outstr, srhstr) != NULL ) {
			rc = getStringFromIndexWithDelim( outstr, ":", 2, 
                                              outstr2, sizeof(outstr2));
			if( rc != FALSE ) {
				strcpysafe( buf, buflen, outstr2 );
				cret = buf;
				break;
			}
		}
	}
	return cret;
}



/*
 * ����饯���ξ��ͥ�٥���Ȥˡ���Τ����Ȥ�����Ψ����롣
 * �֤��ͤ��Ȥ�Ȥ����ʤˤ���������Ф褤��
 * �֤��ͤϥץ쥤�䡼�����ξ���0.2�ʾ�0.6�ʲ��ǡ���٥뤬��
 * ���ۤ������ƺǹ��0.6�ˤʤ롣 NPC��Ź�ξ��Ͼ��1.0��
 * 
 * ����
 * int seller����Τ���륭���Υ���ǥ���
 * �֤���
 * double �� ��Ψ��
 */
inline double NPC_Util_sellRate( int seller )
{
   	return (CHAR_getInt( seller, CHAR_MERCHANTLEVEL)&0x0000ffff)*0.01;
}

/*
 * ����饯���ξ��ͥ�٥���Ȥˡ���Τ��㤦�Ȥ�����Ψ����롣
 * �֤��ͤ��Ȥ�Ȥ����ʤˤ���������Ф褤��
 * �֤��ͤϥץ쥤�䡼�����ξ��0.6�ʾ�1.0�ʲ��ǡ���٥뤬��
 * ���ۤɸ��äƥ�٥�ǹ���ˤ�0.6�ˤʤ롣NPC��Ź�Ͼ��1.0
 * 
 * ����
 * int buyer����Τ��㤦�����Υ���ǥ���
 * �֤���
 * double �� ��Ψ��
 */
inline double NPC_Util_buyRate( int buyer )
{
    return (CHAR_getInt( buyer, CHAR_MERCHANTLEVEL )>>16)*0.01;
}

/*-----------------------------------------------------------------------
 * �ץ쥤�䡼��NPC�򸫤��Ƥ��뤫�򸡺�����
 * BOOL NPC_Util_IsVisiblePlayer( int meindex)
 *
 * IN		int		meindex		�����index
 * OUT		BOOL				TRUE	�������ϰϤ˥ץ쥤�䡼������
 *								FALSE	���ʤ�
 *---------------------------------------------------------------------*/
BOOL NPC_Util_IsVisiblePlayer( int meindex)
{
	int		chr_fl,chr_x,chr_y, i, j;
	BOOL	found = FALSE;
	
	chr_fl = CHAR_getInt( meindex, CHAR_FLOOR);
	chr_x  = CHAR_getInt( meindex, CHAR_X);
	chr_y  = CHAR_getInt( meindex, CHAR_Y);
	
    for( i = chr_x-CHAR_DEFAULTSEESIZ/2 ; 
    	i <= chr_x+CHAR_DEFAULTSEESIZ/2 && found == FALSE; i++ ){
        for( j = chr_y-CHAR_DEFAULTSEESIZ/2 ; 
        	j <= chr_y+CHAR_DEFAULTSEESIZ/2 && found == FALSE;
             j ++ ){
            OBJECT  object;
			for( object = MAP_getTopObj( chr_fl, i,j);
				object;
				object = NEXT_OBJECT(object) ) 
			{
                int objindex = GET_OBJINDEX( object);
                if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
                	if( CHAR_getInt( OBJECT_getIndex(objindex),
                                 CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )
					{
						found = TRUE;
						break;
					}
				}
			}
		}
	}
#if 0
	/* ���˸������ϰϤΤߤ�Ĵ�٤롣*/
	print( "xy[%d,%d]\n", chr_x,chr_y);
	for( k = 0; k < 2; k ++) {
	for( i = 0; i< CHAR_DEFAULTSEESIZ/2+k && found == FALSE; i ++ ) {
		for( j = 0; j < CHAR_DEFAULTSEESIZ/2+k && found == FALSE; j ++){

			OBJECT object;
			print( "[%d,%d]",
									chr_x - i + j,
									chr_y - CHAR_DEFAULTSEESIZ/2+i+j + 
											(k == 0 ? -1:0));
			for( object = MAP_getTopObj( chr_fl, 
									chr_x - i + j,
									chr_y - CHAR_DEFAULTSEESIZ/2+i+j + 
											(k == 0 ? -1:0));
				object;
				object = NEXT_OBJECT(object) ) 
			{
                int objindex = GET_OBJINDEX( object);
                if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
                	if( CHAR_getInt( OBJECT_getIndex(objindex),
                                 CHAR_WHICHTYPE ) == CHAR_TYPEPLAYER )
					{
						found = TRUE;
						break;
					}
				}
			}
		}
	}
	}
#endif
/*	if( found) print( "player found!\n");*/
	return( found);
}

/*
 * ����ñ�줬ñ��˴ޤޤ�Ƥ��뤫�ɤ����򤷤�٤�
 * ��äƤ뤳�Ȥ�strstr����
 * char *text : �ƥ�����
 * char *word : Ĵ�٤�ñ��
 */
BOOL NPC_Util_WordInclude( char *text , char *word )
{
    if( strstr( text , word ) == NULL ){
        return FALSE;
    } else {
        return TRUE;
    }
}
   

/*
 * NPC�Ѥδ�ñ���Ϥʤ�
 * ����������� ʸ������ 1024ʸ���ޤ�
 * int charindex : �����
 * char *msg : ��å�����
 */
void NPC_Util_Talk( int charindex , char *msg )
{
    char fuck[1024];  /* arbitrary */
    snprintf( fuck , sizeof( fuck ), "p|%s" , msg );
    
    CHAR_Talk( -1,charindex , fuck , CHAR_COLORWHITE );
}

/*
 * ����ޤǶ��ڤ�줿ʸ���󤫤������ǥȡ�������������
 * ������Ǥ����Ĥ��Υ�å�����������NPC�ѡ�
 * ��  in "���ʤ�,����,����"
 *     out "����" 
 * char *in:����ޤǶ��ڤ�줿ʸ����
 * char *out:����������Ф줿ʸ����
 * int outbufsize:out�ΥХåե�������
 */
void NPC_Util_RandomToken(char *in, char *out, int outbufsize )
{
    int tokennum;
    int i;

    tokennum = 1;
    /* ����ޤǶ��ڤ�줿�ȡ����󤬲������뤫������ */
    for( i=0;in[i]!='\0';i++ ){
        if( in[i] == ',' ) tokennum++;
    }

    /* ������Ǥɤ�����뤫���ơ����Υȡ����������� */
    getStringFromIndexWithDelim(in,",", rand()%tokennum+1,out,outbufsize );

}

/*
 * ��å������������ΥɥåȤ򤹤٤ƺ�롣
 * ����ϼ�˥ѥ���ɤ�ɥ��ˤϤʤ�������Ȥ��ˡ��ɥåȤ�
 * �ѥ���ɤΰ����Ȥ���ǧ������ʤ��褦�ˤ��뤿��Τ�Τ���
 *  Ϳ����줿ʸ�����ľ�ܥ��ǥ��åȤ��뤾 by ringo
 *
 */
void cutDotsTail( char *s )
{
    int l = strlen( s );
    int i;

    for(i=l-1;i>=0;i--){
        if( s[i] == '.' ){
            s[i] = 0;
        } else {
            break;
        }
    }
        
}
