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
 * NPC、特に敵で汎用につかえる便利関数を定義しまくり
 *ってかんじ
 *
 *
 * とりあえずいま必要なのは、
 *
 *
 * o 近くのプレイヤーを探す速くてまんべんない方法
 *
 * o 無駄に攻撃しないための いい判定関数
 *
 * o ワープさすらいNPCの移動用関数
 *
 *
 * 
 */



/*
 * 称号をいっこ与える
 *
 * int charindex : キャラのindex
 * int titleindex : 称号の種類を特定するindex
 *
 * 成功したらTRUE 失敗したらfalse
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
 * 称号をもってるかどうかしらべる
 *
 * 0 :もってない
 * 1 : もってる
 * 負 : Error
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
 * ふたつのグリッドが隣りあってるかどうかをしらべる。
 *
 * int x1 , y1 : ひとつめ
 * int x2 , y2 : ふたつめ
 *
 * となりあってたら TRUE , そうじゃないならFALSE
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
 * ふたつのキャラが隣りあってるかどうかをしらべる。
 * 絶対値をみるので、同じ位置でも隣りだと認定される
 *
 * int ind1 , ind2 : ふたつのキャラクタのindex
 *
 *
 * となりあってたら TRUE , そうじゃないならFALSE
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
 * YESかNOかを判定する。
 *
 * "はい" "いいえ" "yes" "no" "○" "×"に反応する。
 *  "ＹＥＳ" "Ｙｅｓ" "ｙｅｓ" "Yes" "YES" "yes"
 *
 *
 * 返り値：  0 : no 1 : yes -1 : どちらでもない
 */
static struct ynset
{
    int value;
    char string[32];
}yntable[] ={
    { 0 , "no" },
    { 0 , "No" },
    { 0 , "NO" },
    { 0 , "ＮＯ" },
    { 0 , "Ｎｏ" },
    { 0 , "ｎｏ" },
    { 0 , "いいえ" },
    { 0 , "イイエ" },

    { 1 , "yes" },
    { 1 , "Yes" },
    { 1 , "YES" },
    { 1 , "ＹＥＳ" },
    { 1 , "Ｙｅｓ" },
    { 1 , "ｙｅｓ" },
    { 1 , "はい" },
    { 1 , "ハイ" },
    
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
  ここから下のは、char.cから移動した。
  ============================================================*/
/*------------------------------------------------------------
 * 2点の point を与えて、そのポイント間の直線方向を得る
 * ななめ方向を優先的にみるので、最適ではない。
 * きたないやりかた。
 * 引数
 *  pstart  POINT*  始点
 *  pend    POINT*  終点
 * 返り値
 *  -1  同じ地点
 *  方向
 ------------------------------------------------------------*/
int NPC_Util_getDirFromTwoPoint( POINT* pstart, POINT* pend )
{
    /*  3x3 の表を持つ  */
    /*  y*x */
    static int     dirtable[3][3]={
        {7, 0, 1},
        {6,-1, 2},
        {5, 4, 3},
    };

    int difx, dify;
    difx = pend->x - pstart->x;
    dify = pend->y - pstart->y;

    /*  正規化する  */
    if( difx < 0 )difx=-1;
    else if( difx > 0 )difx=1;
    if( dify < 0 )dify=-1;
    else if( dify > 0 )dify=1;

    return dirtable[dify+1][difx+1];
}

/*------------------------------------------------------------
 * キャラが、指定したIDをもつアイテムをいくつ持っているかを調べる。
 * これはドアNPCが鍵をもっているかどうかを調べるために必要な関数
 * である。
 * 引数
 *  meindex     int     キャラのindex
 *  itemid      int     アイテムのID itemset.txtに書かれてる[ID]
 * 返り値
 *  そのアイテムをもっている個数
 *  負はエラー
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
 * キャラクターがもっているアイテムを1個抹消する。そのアイテムは
 * アイテムIDで指定する。アイテム表を前から検索していって、
 * 最初にヒットしたやつを削除する。
 * 
 * int charindex : キャラクタのindex
 * int itemid : アイテムのID . itemset.txt に記述されている。
 * BOOL net:    ネットワークをするかどうか。TRUEだとする。
 *
 * アイテムをもっていて削除できたら TRUE , 削除できなかったらFALSE
 */
BOOL NPC_Util_RemoveItemByID( int charindex, int itemid, BOOL net )
{
    int i;

    for(i=0;i<CHAR_MAXITEMHAVE;i++){
        int ii = CHAR_getItemIndex( charindex , i);
        if( ITEM_CHECKINDEX(ii)
            && ITEM_getInt( ii, ITEM_ID ) == itemid){
            /* キャラクタがもっているアイテムを*/
            CHAR_setItemIndex( charindex , i , -1 );
            /*
             * そのアイテムを世界から消去する。
             * つまりアイテム表から抹消する
             */
            ITEM_endExistItemsOne( ii );
            /*  両手持ち武器    */
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
 * ふたつのキャラクタを指定して、ひとつのキャラクタがもうひとつ
 * のキャラクタのぴったり後ろにいるかどうかを判定する。
 * 向きも、きちんと前のキャラの背中に向いているかどうかを調べる。
 * 引数
 *  frontindex      int     前にいるとされるキャラ
 *  backindex       int     後ろにいるとされるキャラ。
 * 返り値
 *  backindex が frontindex のうしろにはりついてたら、TRUEを
 *  かえす。そうでない場合FALSEをかえす。
 *
 * この関数は、ドアNPCなどの判定に使える。
 * by ringo
 ------------------------------------------------------------*/
BOOL NPC_Util_isBackContact( int frontindex , int backindex )
{
    /* 方向は、4をたして8で %すると後ろになるので */
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
 * ふたつのキャラクタが直線上に向かいあっていて、かつ
 * 距離があるグリッド以内かどうかを調べる。店用に作ったが,
 * むかいあったらレーザーを打つ敵とかにも使える。
 *
 * 引数
 * int index1, index2 : しらべるふたりのキャラのindex。
 * int distance : ふたりのあいだの距離
 * 返り値
 * むかいあっていて距離がdistance以内だったらTRUE, それ以外はFALSE
 * 重なっていてもFALSE
 */
BOOL NPC_Util_isFaceToFace( int index1, int index2, int distance )
{
    int i;
    if(CHAR_getInt(index1,CHAR_FLOOR) != CHAR_getInt(index2,CHAR_FLOOR)){
        /* フロアが違ったら問題外でFALSE */
        return FALSE;
    }
    if(CHAR_getInt(index1,CHAR_X) == CHAR_getInt(index2,CHAR_X )
       && CHAR_getInt(index1,CHAR_Y) == CHAR_getInt(index2,CHAR_Y) ){
        /* かさなっていたらFALSE */
        return FALSE;
    }
    
    for( i=1;i<=distance;i++ ){
        /* まず、いっこめのキャラが歩いた後の座標を求める */
        int walked_x, walked_y;
        CHAR_getCoordinationDir( CHAR_getInt(index1,CHAR_DIR)
                                 , CHAR_getInt(index1,CHAR_X)
                                 , CHAR_getInt(index1,CHAR_Y)
                                 ,i, &walked_x, &walked_y );
        if( CHAR_getInt(index2,CHAR_X) == walked_x
            && CHAR_getInt(index2,CHAR_Y) == walked_y ){
            /* もういっこのキャラと重なったら、もういっこを同じ
             歩数歩かせた座標を求める。そして重なったらTRUE、
             重ならなかったらFALSE*/
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
    /* ぜんぜん重ならなかったらFALSE */
    return FALSE;
}


/*
 * あるキャラがあるキャラの真正面の何歩か以内にいるか
 * どうか調べる
 *
 * 引数
 * int index1,index2:index1のキャラの前にindex2がいるか
 * どうかを調べる。index1は向きが関係あるけど、index2は関係ない。
 * 返り値
 * 距離がdistance以内だったらTRUE, それ以外はFALSE
 * 重なっていてもFALSE
 */
BOOL NPC_Util_charIsInFrontOfChar( int index1, int index2, int distance )
{
    int i;
    if(CHAR_getInt(index1,CHAR_FLOOR) != CHAR_getInt(index2,CHAR_FLOOR)){
        /* フロアが違ったら問題外でFALSE */
        return FALSE;
    }
    if(CHAR_getInt(index1,CHAR_X) == CHAR_getInt(index2,CHAR_X )
       && CHAR_getInt(index1,CHAR_Y) == CHAR_getInt(index2,CHAR_Y) ){
        /* かさなっていたらFALSE */
        return FALSE;
    }
    
    for( i=1;i<=distance;i++ ){
        /* index1が一歩歩いた先の座標をもとめていく */
        int walked_x, walked_y;
        CHAR_getCoordinationDir( CHAR_getInt(index1,CHAR_DIR)
                                 , CHAR_getInt(index1,CHAR_X)
                                 , CHAR_getInt(index1,CHAR_Y)
                                 ,i, &walked_x, &walked_y );
        if( CHAR_getInt(index2,CHAR_X) == walked_x
            && CHAR_getInt(index2,CHAR_Y) == walked_y ){
            /* index2と重なったらreturn TRUE */
            return TRUE;
        }
    }
    /* ぜんぜん重ならなかったらFALSE */
    return FALSE;
    
}




/*
 * フロアを指定して、そのフロアにいるキャラ全員に同じメッセージ
 * を送信する関数。船面とかに使うのだ
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
 * 既存のアイテムをあるキャラに持たせる。
 * もちものがいっぱいかどうか、スキルの判定、等は
 * この関数の中でやる。
 * 
 * int charindex:もたせるキャラのキャラindex
 * int itemindex:もたせるアイテムのアイテムindex
 * BOOL net:    この関数にネットワークやらせる TRUEにする事。
 * 返り値:無事持たせれたらTRUE,なんらかの原因で持たせれなかったらFALSE
 *  
 * by nakamura
 *
 *  この関数の中で、キャラが持っているものだったら持ちものから外す、
 * 地面に置いてあるものだったら置いてなくする、という処理もしてしまう
 * ようにした。
 * by ringo
 *
 * some bug fix.( ITEM_setWorkInt etc.)
 *  by taka
 *
 * 現在の仕様。
 *  o   itemindex のアイテムが自分の持っているアイテムであったら、
 *      return FALSE する
 */
BOOL NPC_Util_moveItemToChar( int charindex, int itemindex,BOOL net )
{
    int emptyindex, oind ,cind;
    emptyindex = CHAR_findEmptyItemBox( charindex );
    if(  emptyindex < 0 )
        /*  アイテム欄に空きがない  */
        return FALSE;

    if( !ITEM_CHECKINDEX( itemindex ) )return FALSE;
        
    /* 持たせる空きはある   */
    /*
     * 次に地形に落ちているアイテムか、それとも、
     * 誰かが持っているアイテムかを調べる
     */
    oind = ITEM_getWorkInt( itemindex , ITEM_WORKOBJINDEX );
    cind = ITEM_getWorkInt( itemindex , ITEM_WORKCHARAINDEX );

    if( oind >= 0 ){
        /* 地形に置かれているものだ。 */
        /* 地形から抹消 */
        CHAR_ObjectDelete( oind );

        /*  アイテムを移動  */
        CHAR_setItemIndex( charindex, emptyindex, itemindex );

        /*  アイテムのWork領域を書きかえる  */
        ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1 );
        ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charindex );

        /*  charindex がプレイヤーだったらアイテム変更ステータス送信    */
        if( net )            CHAR_sendStatusString( charindex, "I");

        return TRUE;

    }else if( cind >= 0 ){
        /* キャラクタが持っているものだ。 */
        int itemindexinchara;

        /*  自分が持っている場合には何もしない  */
        if( cind == charindex )return FALSE;

        /* もってるものを消す */
        itemindexinchara = NPC_Util_SearchItemInChar( cind , itemindex);
        if( itemindexinchara == -1 )return FALSE;

        CHAR_setItemIndex( cind , itemindexinchara , -1 );
        if( itemindexinchara == CHAR_ARM1
            && CHAR_getItemIndex( cind,CHAR_ARM2 )== -2){
            CHAR_setItemIndex(cind,CHAR_ARM2,-1);
        }
        CHAR_complianceParameter(cind);
        if( net ) CHAR_sendStatusString( cind,"I");
            
        /* アイテムを持たせる */
        CHAR_setItemIndex( charindex , emptyindex , itemindex );
        if( net )  CHAR_sendStatusString( charindex,"I");

        /*  アイテムのWork領域を書きかえる  */
        ITEM_setWorkInt( itemindex, ITEM_WORKOBJINDEX, -1 );
        ITEM_setWorkInt( itemindex, ITEM_WORKCHARAINDEX, charindex );

        return TRUE; /* fixed by nakamura(なんもreturnしてなかった) */
            
    }
    return FALSE;
}
/*
 * 地形におかれているもしくはキャラがもっているアイテム 
 * を強制的に地形のある地点に置く。
 *
 * int itemindex : アイテム表への index
 * int fl,x,y : 位置。
 * BOOL net: ネットワークの処理をするかどうか
 * 返り値は FALSE : 失敗 TRUE : 成功
 */
BOOL NPC_Util_moveItemToMap( int itemindex , int fl , int x , int y,
                             BOOL net )
{
    int oind,cind;
    
    if( !ITEM_CHECKINDEX( itemindex ) ) return FALSE;

    oind = ITEM_getWorkInt( itemindex , ITEM_WORKOBJINDEX );
    cind = ITEM_getWorkInt( itemindex , ITEM_WORKCHARAINDEX );

    if( oind >= 0 ){
        /* 地形におかれている場合は単に位置を変更 */
        return MAP_objmove( oind , OBJECT_getFloor(oind),
                     OBJECT_getX(oind),
                     OBJECT_getY(oind),
                     fl,x,y);
    } else if( cind >= 0 ){
        /* キャラがもっている */
        int itemindexinchara = NPC_Util_SearchItemInChar(cind,itemindex);
        if( itemindexinchara == -1 ) return FALSE;

        /* キャラがもっていないようにする */
        CHAR_setItemIndex( cind , itemindexinchara , -1 );
        if( itemindexinchara == CHAR_ARM1
            && CHAR_getItemIndex( cind,CHAR_ARM2 )== -2){
            CHAR_setItemIndex(cind,CHAR_ARM2,-1);
        }
        CHAR_complianceParameter(cind);
        if( net ) CHAR_sendStatusString( cind,"I");

        /* アイテムindexから直接起く */
        if( CHAR_DropItemAbsolute( itemindex, fl,x,y,TRUE) < 0 ){
            return FALSE;
        } else {
            return TRUE;
        }
    }
    return FALSE;
}


/*
 * キャラがもっているアイテム全部をほかのキャラに移行する。
 * そのときに移行しきれない分はのこる
 *
 * int give : わたしもと
 * int take : わたされ
 * return :
 *  わたしたこすう
 *  負はエラー
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
        /*  ネットワークをする  */
        CHAR_sendStatusString( give,"I");
        CHAR_sendStatusString( take,"I");
    }
    return count;
}
 
/*
 * アイテムを生成してキャラに持たせる。
 * もちものがいっぱいかどうか、スキルの判定、等は
 * この関数の中でやる。
 * int charindex:もたせるキャラのキャラindex
 * int itemid:itemset.txtで設定されているアイテムid
 * BOOL net ネットワークをするかどうか
 *
 * 返り値:無事持たせれたらTRUE,なんらかの原因で持たせれなかったらFALSE
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
        /*  Workデータを設定    */
        ITEM_setWorkInt(itemindex, ITEM_WORKOBJINDEX,-1);
        ITEM_setWorkInt(itemindex, ITEM_WORKCHARAINDEX,charindex);

        if(net)CHAR_sendStatusString(charindex,"I");
        return TRUE;
    }
    return FALSE;
}

/*------------------------------------------------------------
 * クラス指定で、アイテムを作る。
 * 引数
 *  charaindex  int     キャラインデックス
 *  class       int     クラス
 *  net         BOOL    TRUE だとネットワークする
 * 返り値
 *  1       成功
 *  -1      エラー
 *  -2      キャラのアイテム欄がいっぱい
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
 * ふたつのキャラのあいだの距離を求める。
 * ここでの距離とは、2者の座標の差の絶対値
 * の大きいほう。
 * また、floorが違う場合はすごく大きい値を返す。
 * そうすることでfloorの違いを上位ルーチンで
 * あまり意識しなくて良くなるがあまり良い実装では
 * 無いかもしれない。だれか改良して。
 *
 * int index1, index2:距離を求めたいキャラのindex
 * 返り値:距離そのもの
 *
 * by nakamura
 */
int NPC_Util_CharDistance( int index1, int index2 )
{
    int dx, dy;

    /* floorがちがう場合は、じゅうぶんに遠いという意味で
       intの最大値の0x7fffffffを返す。*/ 

    if( CHAR_getInt(index1,CHAR_FLOOR)!= CHAR_getInt(index2,CHAR_FLOOR) ){
        return 0x7fffffff;
    }
    
    dx  =ABS(  CHAR_getInt(index1,CHAR_X)- CHAR_getInt(index2,CHAR_X) );
    dy  =ABS(  CHAR_getInt(index1,CHAR_Y)- CHAR_getInt(index2,CHAR_Y) );

    return max( dx, dy );

}
/*------------------------------------------------------------
 * 一番近いキャラの検索
 *
 * プレイヤー検索から引数でプレイヤーか敵かを検索するようにした
 *  NPC_Util_SearchNearPlayer からNPC_Util_SearchNearに変更
 *  NPC_Util_SearchNearPlayer はNPC_Util_SearchNearを呼び出して
 *  いるので今までとインターフェイスは変わっていない。（河田）
 *
 *	引数：	int meindex 自分のindex
 *			int maxlen	検索範囲（ グリッド数 ）
 *          int type    CHAR_TYPEPLAYER, CHAR_TYPEENEMY
 *
 *	返り値：
 *      一番近い敵のindex
 *      エラーの時は -1
 ------------------------------------------------------------*/
static int  SearchNearLine( /* SearchNearAround からのサブ */
	int xStart, /* 検索するＸ座標の最初 */
	int yStart, /* 検索するＹ座標の最初 */
	int floor,  /* 検索するフロア */
	int xPlus,  /* Ｘ増分 */
	int yPlus,  /* Ｙ増分 */
	int Count,  /* 何回検索するか */
	int type    /* 検索する CHAR タイプ */
){
	OBJECT object;
	int i, x = xStart, y = yStart;

	for( i = 0; i < Count; i ++ ){
		for( object=MAP_getTopObj( floor, x,y ); object ; 
			object = NEXT_OBJECT(object) )
		{
			int objindex = GET_OBJINDEX(object), iWork;
			/* 同じ座標にキャラクタがあるか */
    		if( OBJECT_getType(objindex) != OBJTYPE_CHARA )continue;
    		iWork = OBJECT_getIndex(objindex);
    		/* ヒットポイントマイナスは無視 */
    		if( CHAR_getInt(iWork,CHAR_HP) < 0 )continue;
			/* そのキャラクタは欲しいタイプか？ */
			if( CHAR_getInt( iWork,CHAR_WHICHTYPE ) == type ){
			/* ついに発見 */
				return iWork;
			}
		}
		/* 次のフィールドをさす */
		x += xPlus;	y += yPlus;
	}
	return -1;
}


static int SearchNearAround( 
	int x, 	   /* 検索中心のＸ座標 */
	int y,     /* 検索中心のＹ座標 */
	int floor, /* 検索中心のフロア */
	int Part,  /* 検索開始する最初のライン */
	int Level, /* 中心より離れている距離 */
	int type   /* 検索する CHAR タイプ */
){
	int i, iTarget = -1;
	for( i = 0; i < 4; i ++ ){
		if( Part == 0 ){ /* 上のラインを検索 */
			iTarget = SearchNearLine( x-Level+1, y-Level, 
				floor, 1, 0, Level*2, type );
			if( iTarget >= 0 )break;
		}else
		if( Part == 1 ){ /* 右のラインを検索 */
			iTarget = SearchNearLine( x+Level, y-Level+1, 
				floor, 0, 1, Level*2, type );
			if( iTarget >= 0 )break;
		}else
		if( Part == 2 ){ /* 下のラインを検索 */
			iTarget = SearchNearLine( x+Level, y+Level, 
				floor, -1, 0, Level*2, type );
			if( iTarget >= 0 ) break;
		}else
		if( Part == 3 ){ /* 左のラインを検索 */
			iTarget = SearchNearLine( x-Level, y+Level, 
				floor, 0, -1, Level*2, type );
			if( iTarget >= 0 ) break;
		}
		Part ++; Part &= 3; /* 次のパートを検索 */
	}
	return iTarget;
}

int NPC_Util_SearchNear( int meindex, int maxlen, int type )
{
#if 1
	/* MAP からのリンクで検索するように変更 */
    int	floor, x, y, i,iTarget = -1;
	/* 現在の自分の情報を引き出す */
    if( !CHAR_CHECKINDEX( meindex ) )return -1;
    floor = CHAR_getInt( meindex, CHAR_FLOOR );
    x  = CHAR_getInt( meindex, CHAR_X );
    y  = CHAR_getInt( meindex, CHAR_Y );

	/* 最大範囲内で調べる */
	for( i = 0; i < maxlen; i ++ ){
		iTarget = SearchNearAround( x, y, floor, RAND( 0,3 ), i+1, type );
		if( iTarget >= 0 )break;
	}
	return iTarget;


#else
	/* こちらは古いやり方 */
    int	floor, x, y, i,iTarget = -1, iLen, iMin = 655360, tX, tY;
    /* 変数ゲット*/
    if( !CHAR_CHECKINDEX( meindex ) )return -1;
    floor = CHAR_getInt( meindex, CHAR_FLOOR );
    x  = CHAR_getInt( meindex, CHAR_X );
    y  = CHAR_getInt( meindex, CHAR_Y );

    /*  検索 */
    for( i = 0 ; i<objnum ; i++ ){
        /*キャラクターの時*/
        if( obj[i].type != OBJTYPE_CHARA ) continue;
		/* 種別を見る*/
        if( CHAR_getInt( obj[i].index,CHAR_WHICHTYPE)
            != type ) continue;
		/* 同じフロアの時*/
	    if( floor != obj[i].floor ) continue; 
	    
	    /* CHAR_TYPEENEMY の時はindexが自分の場合があるのでチェックする */
        if( obj[i].index == meindex ) continue;

		/* ＸＹの距離を計算*/
		tX = ABS( x-CHAR_getInt(obj[i].index,CHAR_X) );
		tY = ABS( y-CHAR_getInt(obj[i].index,CHAR_Y) );
		if( tX > maxlen )continue;	/* 離れすぎ*/
		if( tY > maxlen )continue;	/* なら次へ*/

        /* 細かい距離を計算*/
		iLen = tX*tX+tY*tY;
		/* 一番近かった場合は記録更新*/
		if( iMin > iLen ){
			iMin = iLen;
			iTarget = obj[i].index;	/* インデックスを登録*/
		}
	}
	
	return iTarget;
#endif
}
/*------------------------------------------------------------
* 一番近いプレイヤーの検索
------------------------------------------------------------*/
int NPC_Util_SearchNearPlayer( int meindex, int maxlen )
{
	return( NPC_Util_SearchNear( meindex, maxlen, CHAR_TYPEPLAYER));
}
/*------------------------------------------------------------
* 一番近い敵の検索-
-----------------------------------------------------------*/
int NPC_Util_SearchNearEnemy( int meindex, int maxlen )
{
	return( NPC_Util_SearchNear( meindex, maxlen, CHAR_TYPEENEMY));
}



/*******************************************************

  斜め移動する前に何かにぶつかっているかどうか調べる
  int NPC_Util_SuberiWalk( 
	int	index,	CHAR のインデックス
	int	dir		歩く向き
  );

  戻り値  0〜7：歩ける方向
          -1  ：マップに引っかかってエラー
          -2  ：キャラクタに引っかかってエラー
          -3  ：両方に引っかかってエラー

  斜め移動する場合まず縦、横に移動できるか調べる。
  両方移動できれば斜めをチェックする。片方しか移動できなければ
  そちらの方向を返す。

 そこに何かあったら（マップ以外）１を返す
*******************************************************/ 
int NPC_Util_OtherCharCheck( int ff, int fx, int fy)
{
#if 1
	/* 検索方式をＭＡＰからのリンクに変更 */
	OBJECT object;
	for( object=MAP_getTopObj(ff,fx,fy); object ; 
		object = NEXT_OBJECT(object) )
	{
		int objindex = GET_OBJINDEX(object);
		/* 同じ座標にキャラクタがあるか */
    	if( OBJECT_getType(objindex) == OBJTYPE_CHARA ){
    		return 1;
		}
	}
	return 0;
#else
	/* こちらは旧式のやり方 */
    int i;
    for( i = 0 ; i < objnum ; i ++){
		/* 座標が同じか？*/
        if( obj[i].x == fx && obj[i].y == fy && obj[i].floor == ff ){
        	/* キャラクタか？*/
	        if( obj[i].type != OBJTYPE_NOUSE ){
	            return 1;
            }
        }
    }
	return 0;
#endif
}


/* 歩けない場合は滑ってみる*/
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

	/* 斜め方向か？*/
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
		/* 進めた*/
		if( MAP_walkAble( index,fl,tX,tY ) == 0 ){
			dir = -1;/* マップに引っかかった*/
		}else
		if( NPC_Util_OtherCharCheck( fl,tX, tY ) ){
			dir = -2;/* キャラに引っかかった*/
		}
		/* まっすぐだ!! そのまま歩く*/
/*		CHAR_walk( index, dir, 0 );*/
		return dir;
	}

	/* まっすぐなら判定なし*/
	for( i = 0; i < 3; i ++ ){
		dir2 = aDirList[i];
		tX = x+CHAR_getDX(dir2);
		tY = y+CHAR_getDY(dir2);
		if( MAP_walkAble( index,fl,tX,tY ) == 0 ){
			aDirList[i] = -1; /* 行けない場合はその方向抹殺*/
		}else if( NPC_Util_OtherCharCheck( fl,tX, tY ) ){
			aDirList[i] = -2; /* 行けない場合はその方向抹殺*/
		}else{
			/* 行けた!!*/
			OkFlg ++;
		}
	}

	/* ３つ全部可能な場合は、最初の通り動く*/
	if( aDirList[0] >= 0 && aDirList[1] >= 0 && aDirList[2] >= 0 ){
	}else
        /* こちらに動けるか？*/
	if( aDirList[0] >= 0 ){
		dir = aDirList[0];
	}else
        /* それともこちらか？*/
	if( aDirList[1] >= 0 ){
		dir = aDirList[1];
	}else{
		dir = aDirList[0]+aDirList[1];
	}

	/* 移動*/
/*	CHAR_walk( index, dir, 0 );*/

	return dir;
} 
/*-----------------------------------------------------------------------
* NPCのcreate時の引数の中から指定された文字列を検索し，
* その中の数値を取得する。オプションで数値を指定させてやりたいときに便利
* 
* 注意：デリミタは"|"に限定してます。
*       引数に同じ文字列が２つ並んだ場合，
*       先の方が優先されます。
* ----------------------------------
* IN : int meindex : キャラindex
*    : char* in    : 検索する文字列
* OUT:
* RETURN: 
* 取得出来なかった時 :-1
* 取得出来た時       :数値
* ----------------------------------
*
* 例：
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
		/* デリミタ１つずつ読む */
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
 * アイテムの index とキャラ index のふたつの情報から、
 * キャラがそのアイテムを持ってるかどうかしらべて持ってたらどの
 * 位置に持ってるかを返す。  アイテム関連のNPC用。CHAR_に移行
 * できるかもしれん関数ナリ
 * by ringo
 *
 * int charindex : 対象キャラの index
 * int itemindex : アイテムのindex
 *
 * 返り値
 *
 *  >=0 : どの位置に持っているのか。有効な値。
 *  -1 : もしくは持ってない。
 *  -2 : エラー。
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
 *  持っているアイテムを全部廃棄する。アイテム表から正しくアイテムを
 *  抹消する。
 *  もともとはSimpleShopの初期化失敗用。
 *  店以外にも、アイテム表をすっきりしてから死ぬ(というか必要だ)とき
 *  につかえ。
 *  int charind : 操作対象のキャラクタ。
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
        
        /* アイテム表を消してから */
        ITEM_endExistItemsOne( iind );

        /* 自分のもっているのを消す */
        CHAR_setItemIndex( charind,i ,-1);

        /*  両手持ちの武器の後始末  */
        if( i == CHAR_ARM1 && CHAR_getItemIndex(charind,CHAR_ARM2) == -2 )
            CHAR_setItemIndex( charind, CHAR_ARM2, -1);
    }
}
/*------------------------------------------------------------------------
* キャラからみた他のキャラの方向を求める
*
* int NPC_Util_GetDirCharToChar( int fromindex, int toindex, int mode)
*
* int	fromindex 	このキャラindexから
* int	toindex		このキャラindexに対しての方向が求められる
* int	mode		取得する方向
*                   0:toindexに向かって
*                   1:toindexの反対方向
*                   2:toindexに対して右９０度方向
*                   3:toindexに対して左９０度方向
* 
* 返り値 成功 ：dir（方向を返す）
*        失敗 ：-1
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
* 目標キャラに対して，ある方向に移動する。
*
* int NPC_Util_WalkCharToChar( int fromindex, int toindex, int mode)
*
* int	fromindex 	歩くキャラindex
* int	toindex		歩く目標のキャラindex
* int	mode		歩く方向
*                   0:toindexに向かって
*                   1:toindexの反対方向
*                   2:toindexに対して右９０度方向
*                   3:toindexに対して左９０度方向
* BOOL	suberi		NPC_Util_SuberiWalkを考慮するか？TRUE:する FALSE:しない
* 
* 返り値 成功（歩いた）：dir（歩いた方向を返す）
*        失敗          ：-1
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
 *  NPC間通信用。
 *
 *  他のNPCをコントロールする一般的な関数。
 * 文字列を引数に取り、ややこしいことを一手にひきうける。
 * たとえば、ドアが NPC_DoorOpen を持っているかとかを
 * こいつは知っている。
 *
 *
 * CHAR_TYPE chartype : キャラの種類ごとに分類しているenum
 * char *npcname : NPCの検索名
 * char *command : コマンド
 *
 * 現在command は以下の文字列に対応している。
 *
 * open : NPC_???Open をよぼうとする。
 * close : NPC_???Close をよぼうとする。
 * flip : NPC_???flip をよぼうとする。
 * counter+N : NPC_????AddSwitchCounterをよぼうとする。
 * counter-N : NPC_????AddSwitchCounterをよぼうとする。
 * counter=N : NPC_????SetSwitchCounter をよぼうとする。
 * kill : NPC_????KillByOtherNPC をよぼうとする。
 * fillitembyid=ID : NPC_????FillItemByID をよぼうとする。
 * removeitembyid=ID : NPC_????RemoveItemByID をよぼうとする。
 *
 * return value:
 *  -1 : そんなNPCはいない
 *  -2 : chartypeがおかしい
 *  -3 : command を理解できない。
 *  -4 : そのNPCはまだサポートしてない。
 *   0 : 成功
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
        /* まずドアを検索 */
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
 * ACTION のenum の値のどれかを得る
 * 
 * char *com : コマンド全体がはいってる文字列
 * STRING64 *opt:  オプションの文字列。
 * return value:
 * CTRLOTHERNPC_???? を得る
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
* NPCを消滅させる。
*
* void NPC_Util_NPCDelete( int srcindex)
*
* int	srcindex 	消すindex
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
 * create引数にArgFileの指定があるか調べる
 * static char *NPC_Util_CheckAssignArgFile( int index, char *filename)
 *
 * IN		int		index		キャラindex
 * OUT		char	*filename	ファイル名
 *
 * RETURN	成功	filenameのアドレス
 *			失敗	NULL
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
		/* デリミタ１つずつ読む */
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
 * ArgFileの中身を"|"で連結して文字列で返す
 * static char *NPC_Util_MargeStrFromArgFile( char *filename, char *buf)
 *
 * IN		char	*filename	開くファイル名
 *			int		len			bufのサイズ
 * OUT		char	*buf		連結された引数
 *
 * RETURN	成功	bufのアドレス
 *			失敗	NULL
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
 * 引数ファイルかもとの引数から引数を取得する
 * char *NPC_Util_GetArgStr( int index, char *argstr)
 * 
 * IN		int		index		キャラindex
 *          int		len			argstrのサイズ
 * OUT		char	argstr		引数の文字列
 *
 * RETURN	成功	argstrのアドレス
 *			失敗	NULL
 *----------------------------------------------------------------------*/
char *NPC_Util_GetArgStr( int index, char *argstr, int len)
{
	char	filename[32];
	char	*cret = NULL;
	
	/* ファイルを調べる*/
	if( NPC_Util_CheckAssignArgFile( index, filename) != NULL ) { 
		/* 引数を得る */
		if( NPC_Util_MargeStrFromArgFile( filename, argstr, len) != NULL ) {
			cret = argstr;
		}
	}
	/* なければ.createファイルの引数をそのまま返す */
	else {
		strcpy( argstr, CHAR_getChar( index, CHAR_NPCARGUMENT));
		cret = argstr;
	}
	return( cret);
}

/*-----------------------------------------------------------------------
* デリミタ("|")で区切られた文字列の中から指定された文字列を検索し，
* その中の数値を取得する。
* 
* 注意：デリミタは"|"に限定してます。
*       引数に同じ文字列が２つ並んだ場合，
*       先の方が優先されます。
* ----------------------------------
* IN : char *srcstr	: 検索元文字列
*      char *in     : 検索する文字列
* OUT:
* RETURN: 
* 取得出来なかった時 :-1
* 取得出来た時       :数値
* ----------------------------------
*----------------------------------------------------------------------*/
int NPC_Util_GetNumFromStrWithDelim( char *srcstr, char* in)
{
	BOOL    rc;
	int     i;
	char    outstr[32];
	int		out = -1;
	
	for( i = 1; ; i++ ) {
		/* デリミタ１つずつ読む */
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
 * という文字列の中から"bbb"を指定すると"ccc"を取ってこれる関数
 * char *NPC_Util_GetStrFromStrWithDelim( char *srcstr, 
 *										  char *srhstr, 
 *										  char *buf)
 *
 * IN	char *srcstr		検索元文字列
 *		char *srhstr		探す文字列
 *		int	  len			格納バッファのサイズ
 * OUT	char *buf			格納するバッファ
 *
 * RETURN	成功			bufのアドレス
 *			失敗			NULL
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
		/* デリミタ１つずつ読む */
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
 * キャラクタの商人レベルをもとに、ものを売るときの倍率を求める。
 * 返り値をもともとの値段にかけ算すればよい。
 * 返り値はプレイヤーキャラの場合は0.2以上0.6以下で、レベルが高
 * いほど増えて最高で0.6になる。 NPCの店の場合は常に1.0。
 * 
 * 引数
 * int seller：ものを売るキャラのインデクス
 * 返り値
 * double ： 倍率。
 */
inline double NPC_Util_sellRate( int seller )
{
   	return (CHAR_getInt( seller, CHAR_MERCHANTLEVEL)&0x0000ffff)*0.01;
}

/*
 * キャラクタの商人レベルをもとに、ものを買うときの倍率を求める。
 * 返り値をもともとの値段にかけ算すればよい。
 * 返り値はプレイヤーキャラの場合0.6以上1.0以下で、レベルが高
 * いほど減ってレベル最高時には0.6になる。NPCの店は常に1.0
 * 
 * 引数
 * int buyer：ものを買うキャラのインデクス
 * 返り値
 * double ： 倍率。
 */
inline double NPC_Util_buyRate( int buyer )
{
    return (CHAR_getInt( buyer, CHAR_MERCHANTLEVEL )>>16)*0.01;
}

/*-----------------------------------------------------------------------
 * プレイヤーがNPCを見えているかを検索する
 * BOOL NPC_Util_IsVisiblePlayer( int meindex)
 *
 * IN		int		meindex		キャラindex
 * OUT		BOOL				TRUE	見える範囲にプレイヤーがいる
 *								FALSE	いない
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
	/* 純粋に見える範囲のみを調べる。*/
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
 * ある単語が単純に含まれているかどうかをしらべる
 * やってることはstrstrだ。
 * char *text : テキスト
 * char *word : 調べる単語
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
 * NPC用の簡単おはなし
 * 色は白だけ、 文字数は 1024文字まで
 * int charindex : キャラ
 * char *msg : メッセージ
 */
void NPC_Util_Talk( int charindex , char *msg )
{
    char fuck[1024];  /* arbitrary */
    snprintf( fuck , sizeof( fuck ), "p|%s" , msg );
    
    CHAR_Talk( -1,charindex , fuck , CHAR_COLORWHITE );
}

/*
 * コンマで区切られた文字列からランダムでトークンを取りだす。
 * ランダムでいくつかのメッセージを喋るNPC用。
 * 例  in "うなぎ,すし,弁当"
 *     out "すし" 
 * char *in:コンマで区切られた文字列
 * char *out:ランダムで選ばれた文字列
 * int outbufsize:outのバッファサイズ
 */
void NPC_Util_RandomToken(char *in, char *out, int outbufsize )
{
    int tokennum;
    int i;

    tokennum = 1;
    /* コンマで区切られたトークンが何こあるか数える */
    for( i=0;in[i]!='\0';i++ ){
        if( in[i] == ',' ) tokennum++;
    }

    /* ランダムでどれを喋るか決めて、そのトークンを取りだす */
    getStringFromIndexWithDelim(in,",", rand()%tokennum+1,out,outbufsize );

}

/*
 * メッセージの末尾のドットをすべて削る。
 * これは主にパスワードをドアにはなしかけるときに、ドットが
 * パスワードの一部として認識されないようにするためのものだ。
 *  与えられた文字列を直接エディットするぞ by ringo
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
