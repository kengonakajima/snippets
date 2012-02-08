#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

//#define TRUE 1
//#define FALSE 0
//#define DATANUM 10

#define		LINENUM		10

#define		STRING_CAPTION		"caption"
#define		STRING_LINE			"line"
#define		STRING_SCRIPT		"script"
#define		STRING_COMMENT		"comment"
#define		STRING_SURE			"sure"

#define		WMAIN_WIDTH			40
#define		WMAIN_HEIGHT		20
#define		WMAIN_X				1
#define		WMAIN_Y				3

#define		WCOMMENT_WIDTH		36
#define		WCOMMENT_HEIGHT		10
#define		WCOMMENT_X			42
#define		WCOMMENT_Y			3

#define		WMSG_WIDTH			40
#define		WMSG_HEIGHT			1
#define		WMSG_X				2
#define		WMSG_Y				23

#define		WTITLE_WIDTH		80
#define		WTITLE_HEIGHT		1
#define		WTITLE_X			0
#define		WTITLE_Y			0

#define		WPAGE_WIDTH			80
#define		WPAGE_HEIGHT		1
#define		WPAGE_X				0
#define		WPAGE_Y				1

#define		WHELP_WIDTH 	36
#define		WHELP_HEIGHT	10
#define		WHELP_X			42
#define		WHELP_Y			13

int menucnt = 0;
int page = 0;
int maxpage;
char title[1024];

WINDOW	*wMain;				//	main window
WINDOW	*wComment;			//  comment window
WINDOW	*wMsg;				//	message window
WINDOW	*wTitle;			//	title window
WINDOW	*wHelp;				//	help window
WINDOW	*wPage;				//	page window

struct tagMenu {
	int		sure;
	char	caption[1024];
	char	script[1024];
	char	comment[1024];
	
}*menu;

//-------------------------------------------------------------------
void exitmain( int number)
{
    signal( SIGINT , SIG_IGN );
    signal( SIGQUIT, SIG_IGN );
    signal( SIGKILL, SIG_IGN );
    signal( SIGSEGV, SIG_IGN );
    signal( SIGTERM, SIG_IGN );
	endwin();
	free( menu);
	exit( 1);
}
//-------------------------------------------------------------------
void deltopspace( char *msg, int msglen)
{
	int i;
	
	for( i = 0; i < msglen; i ++ ) {
		if( msg[i] != ' ' && msg[i] != '\t') break;
	}
	memmove( msg, &msg[i], msglen - i);
	msg[msglen -i] = '\0';
}
//-------------------------------------------------------------------
int cfgread( void)
{
	FILE *fp;
	char buff[1024];
	int line;
	int i;
	
	fp = fopen( "menu.cfg", "r");
	if( fp == NULL ) {
		return FALSE;
	}
	
	line = -1;
	
	// top line is title
	if( fgets( buff, sizeof( buff), fp)) {
		strcpy( title, buff);
	}
	
	// count caption
	while( fgets( buff, sizeof( buff), fp)) {
		// del cr
		buff[strlen(buff)-1] = '\0';
		
		deltopspace( buff, strlen( buff));
		
		// ignore comment
		if( buff[0] == '#' || buff[0] == '\n') continue;
		
		if( strncmp( buff, STRING_CAPTION, strlen( STRING_CAPTION)) == 0 ) {
			line ++;
		}
	}
	
	menucnt = line+1;
	maxpage = line / LINENUM;
	menu = ( struct tagMenu *)malloc( sizeof( struct tagMenu)*menucnt);
	if( menu == NULL ) {
		fclose( fp);
		return FALSE;
	}
	
	// initialize
	for( i = 0; i < menucnt; i ++ ) {
		menu[i].caption[0] = '\0';
		menu[i].script[0] = '\0';
		menu[i].comment[0] = '\0';
		menu[i].sure = TRUE;
	}
	
	fseek( fp, 0, SEEK_SET);
	
	line = -1;
	
	// top line is title
	if( fgets( buff, sizeof( buff), fp)) {
		strcpy( title, buff);
	}
	while( fgets( buff, sizeof( buff), fp)) {
		// del cr
		buff[strlen(buff)-1] = '\0';
		
		deltopspace( buff, strlen( buff));
		
		// ignore comment
		if( buff[0] == '#' || buff[0] == '\n') continue;
		
		if( strncmp( buff, STRING_CAPTION, strlen( STRING_CAPTION)) == 0 ) {
			char *pValue = strtok( buff, "=");
			if( pValue != NULL ) pValue = strtok( NULL, "=");
			line ++;
			if( pValue != NULL ) {
				strcpy( menu[line].caption, pValue);
			}
		}
		else if( strncmp( buff, STRING_SCRIPT, strlen( STRING_SCRIPT)) == 0 ) {
			char *pValue = strtok( buff, "=");
			if( pValue != NULL ) pValue = strtok( NULL, "=");
			if( pValue != NULL && line != -1 ) {
				strcpy( menu[line].script, pValue);
			}
		}
		else if( strncmp( buff, STRING_COMMENT, strlen( STRING_COMMENT)) == 0 ) {
			char *pValue = strtok( buff, "=");
			if( pValue != NULL ) pValue = strtok( NULL, "=");
			if( pValue != NULL && line != -1 ) {
				strcpy( menu[line].comment, pValue);
			}
		}
		else if( strncmp( buff, STRING_SURE, strlen( STRING_SURE)) == 0 ) {
			char *pValue = strtok( buff, "=");
			if( pValue != NULL ) pValue = strtok( NULL, "=");
			if( pValue != NULL && line != -1 ) {
				if( strcmp( pValue, "no") == 0 ) menu[line].sure = FALSE;
			}
		}
	}
	fclose( fp);
	return TRUE;
}
//-------------------------------------------------------------------
int init( void)
{

	initscr();
	clear();
	wMain = newwin( WMAIN_HEIGHT, WMAIN_WIDTH,  WMAIN_Y, WMAIN_X);
	if( wMain == NULL ) return FALSE;
	wComment = newwin( WCOMMENT_HEIGHT, WCOMMENT_WIDTH, WCOMMENT_Y, WCOMMENT_X);
	if( wComment == NULL ) return FALSE;
	wMsg = newwin( WMSG_HEIGHT, WMSG_WIDTH, WMSG_Y, WMSG_X);
	if( wMsg == NULL ) return FALSE;
	wTitle = newwin( WTITLE_HEIGHT, WTITLE_WIDTH, WTITLE_Y, WTITLE_X);
	if( wTitle == NULL ) return FALSE;
	wHelp = newwin( WHELP_HEIGHT, WHELP_WIDTH, WHELP_Y, WHELP_X);
	if( wHelp == NULL ) return FALSE;
	wPage = newwin( WPAGE_HEIGHT, WPAGE_WIDTH, WPAGE_Y, WPAGE_X);
	if( wPage == NULL ) return FALSE;
	signal( SIGINT, exitmain);
	signal( SIGQUIT, exitmain);
	signal( SIGKILL, exitmain);
	signal( SIGSEGV, exitmain);
	signal( SIGTERM, exitmain);
	
	keypad( stdscr, TRUE);
	keypad( wMain, TRUE);
	noecho();
	curs_set(0);
	return TRUE;
}

//-------------------------------------------------------------------
void printtitle( void)
{
	//wmove( wTitle, 0,0);
	
	// alien center
	werase( wTitle);
	mvwaddstr( wTitle, 0,(WTITLE_WIDTH - strlen( title))/2, title);
	wrefresh( wTitle);
}
//-------------------------------------------------------------------
void dspcaption( int y)
{
	int i;
	char  a[3];
	
	werase( wMain);
	for( i = 0; i < LINENUM ; i ++ ) {
		if( i+page*LINENUM >= menucnt ) break;
		wmove( wMain, i*2, 0);
		if( y == i ) {
			wattrset( wMain, A_BOLD);
			strcpy( a, "☆");
		}
		else {
			wattrset( wMain, A_NORMAL);
			strcpy( a, "・");
		}
		wprintw( wMain, "%s %s", a, menu[i+page*LINENUM].caption);
	}
}

//-------------------------------------------------------------------
void dspcomment( int line)
{
	werase( wComment);
	mvwaddstr( wComment, 0,0,menu[line+page*LINENUM].comment);
	wrefresh( wComment);
}
//-------------------------------------------------------------------
void dsphelp( void)
{
	werase( wHelp);
	mvwaddstr( wHelp, 0,0, "カーソルキーの上下，\n");
	waddstr( wHelp, "「２８」「ｊｋ」キーで選択し\n");
	waddstr( wHelp, "エンターキーで実行します。\n");
	waddstr( wHelp, "カーソルの左右，\n");
	waddstr( wHelp, "「４６」「ｈｌ」でページ切り替え。\n");
	waddstr( wHelp, "q又はCtrl+cで終了します。\n");
	
	wrefresh( wHelp);
}
//-------------------------------------------------------------------
void dsppage( void)
{
	werase( wPage);
	mvwprintw( wPage, 0,0, "Page %d/%d", page+1, maxpage+1);
	wrefresh( wPage);
}
//-------------------------------------------------------------------
int main( int argc, char **argv)
{
	int c; 
	int y;
	
	if( !init() ) {
		endwin();
		printf( "init screen failed\n");
		return 0;
	}
	
	if( !cfgread() ) {
		endwin();
		printf( "config file open error\n");
		return 0;
	}
	printtitle();
	dsphelp();
	y = 0;
	dspcaption(y);
	dspcomment( y);
	dsppage();
	while( 1) {
		c = wgetch(wMain);
		switch( c) {
		  case KEY_UP:
		  case '8':
		  case 'k':
			y--;
			break;
		  case KEY_DOWN:
		  case 'j':
		  case '2':
			y++;
			break;
		
		  case KEY_LEFT:
		  case '4':
		  case 'h':
			page --;
			if( page < 0 ) page = 0;
			else {
				dsppage();
			}
			break;
		  case KEY_RIGHT:
		  case '6':
		  case 'l':
			page ++;
			if( page > maxpage ) page = maxpage ;
			else {
				if( y+page*LINENUM >= menucnt) y = 0;
				dsppage();
			}
		  break;
		  
		  case 'q':
			exitmain( 0);
			break;
		  case 10:
		  case 13:
		  case KEY_ENTER:
			werase( wMsg);
			wrefresh( wMsg);
			if( menu[y].sure ) {
				int recv;
				mvwprintw( wMsg, 0,0, "are you sure? [y]\n");
				recv = wgetch( wMsg);
				werase( wMsg);
				wrefresh( wMsg);
				if( tolower(recv) != 'y' ) break;
			}
			
			wprintw( wMsg, " ");
			wrefresh(wMsg);
			system( menu[y+page*LINENUM].script);
			{
				int recv;
				mvwprintw( wMsg, 0,0,"\nhit any key\n");
				recv = wgetch( wMsg);
				erase();
				refresh();
				
				werase( wMsg);
				wrefresh( wMsg);
				werase( wComment);	
				werase( wMain);
				printtitle();
				dsphelp();
				dsppage();
			}
			
		}
		if( y < 0 ) y = 0;
		if( y >= LINENUM ) y = LINENUM -1;
		if( y+page*LINENUM >= menucnt ) y --;
//	move( y,2);
		dspcaption( y);
		dspcomment( y);
	}
	return 0;
}

