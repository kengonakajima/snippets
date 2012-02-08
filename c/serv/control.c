#define _CONTROL_C_

#include <stdio.h>

#include "system.h"
#include "control.h"
#include "log.h"
#include "character.h"
#include "item.h"
#include "main.h"
#include "account.h"
#include "dp.h"
#include "do.h"


/*********************************************
  MULTI THREADED FUNCTION

  全体の管理をする


*********************************************/


void overallControl(void)
{
	static oval_st_store ;

	if( oval_st_store != systemtime ){
		oval_st_store = systemtime;
	} else {
		return;
	}

	/* ログインキャラのタイムアウト処理 */
	checkLoginTimeout();

	/* MOVE パケットの個数について */
	move_packet_count_save = move_packet_count;
	move_packet_count = 0;

	/* パスワード変更関連ルーチン */
	checkChangePasswdTable();

	/* サブバージョン関連 */
	checkSubVersion();
	
	/* 遅延パケット関連 */
	checkDelayedPacketTable();

	/* アイテムのタイムアウト関連 */
	checkItemTimeout( timeout_item_base_name , timeout_item_sec );
}

int checkFaster(void )
{
	if( move_packet_count_save < send_faster_if_less_than ) return 1; else return 0;
}

void checkLoginTimeout( void )
{
	int i;

	
	for(i=0;i<maxaccounts;i++){

			
		if( accounts[i].now_login == ON && ( systemtime - accounts[i].last_message ) > timeout_sec  ){
			char msg[100];
			logoutChar( i , (struct chardata *) NULL , ON );
			sprintf( msg , "TIMEOUT:[%s]\n" , accounts[i].cd_key );
			logger( msg );	/* TIMEOUT LOG */
		}
	}
	

}




/***********************
   サブバージョンのチェック
   
***********************/
#define CHECKSUBVERSION_INTERVAL 120
void checkSubVersion( void )
{
	int i;
	static time_t csv_sstore;
	
	if( csv_sstore != ( systemtime / CHECKSUBVERSION_INTERVAL ) ){
		csv_sstore = systemtime/CHECKSUBVERSION_INTERVAL;
	} else {
		return;
	}
	for(i=0;i<maxaccounts;i++ ){
		if( accounts[i].now_login == ON &&
		    accounts[i].current_subversion != subversionnumber ){
			/* ログインしていてバージョンがちがうから送信だ */
			if( strlen( subvermessage ) > 0 ){
				sendTextCommand( accounts[i].cliaddr  , subvermessage , 0);
			}
#if 0
			if( debug_mode ){
				fprintf( stderr , "Ver: %d %d %x\n" , accounts[i].current_subversion , subversionnumber,
						(int)accounts[i].cliaddr.sin_addr.s_addr );
			}
#endif
			
		}
	}
}
