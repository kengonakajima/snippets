#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <fcntl.h>







int main(int argc ,char **argv )
{
	int i;
	
	initscr();

	
	raw();
	noecho();
	nonl();

	move( 10 , 10);
	addch( 'a' );
	
	refresh();

	endwin();

	i=0;
	
	fcntl( 0 , F_SETFL , FNDELAY );
	
	while(1){
		int c;
		c = getch();

		if( c == 'q' )break;
		

		move( 5,5 );
		printw("%d" , i );
		
		refresh();
		
	}
	

	
#if 0
	
	i = 0 ;
	while(1){
		int c;
		
		char msg[100];
		i++;
		sprintf(msg , "%d" , i );
		move(0,0);
		printw( msg );
		
		c = getch();
		if( c=='a' ) break;
		refresh();
		
	}
#endif
	
	
	


}
