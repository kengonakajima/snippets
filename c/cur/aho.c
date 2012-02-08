#include <curses.h>


int main()
{

	initscr();

	
	raw();
	noecho();
	nonl();

	move( 10 , 10);
	addch( 'a' );
	
	refresh();

    sleep(1);
	endwin();
    
    
}
