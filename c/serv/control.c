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

  $BA4BN$N4IM}$r$9$k(B


*********************************************/


void overallControl(void)
{
	static oval_st_store ;

	if( oval_st_store != systemtime ){
		oval_st_store = systemtime;
	} else {
		return;
	}

	/* $B%m%0%$%s%-%c%i$N%?%$%`%"%&%H=hM}(B */
	checkLoginTimeout();

	/* MOVE $B%Q%1%C%H$N8D?t$K$D$$$F(B */
	move_packet_count_save = move_packet_count;
	move_packet_count = 0;

	/* $B%Q%9%o!<%IJQ994XO"%k!<%A%s(B */
	checkChangePasswdTable();

	/* $B%5%V%P!<%8%g%s4XO"(B */
	checkSubVersion();
	
	/* $BCY1d%Q%1%C%H4XO"(B */
	checkDelayedPacketTable();

	/* $B%"%$%F%`$N%?%$%`%"%&%H4XO"(B */
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
   $B%5%V%P!<%8%g%s$N%A%'%C%/(B
   
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
			/* $B%m%0%$%s$7$F$$$F%P!<%8%g%s$,$A$,$&$+$iAw?.$@(B */
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
