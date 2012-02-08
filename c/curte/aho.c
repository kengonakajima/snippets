#include <stdio.h>
#include <curses.h>


main()
{
	int i;
	int x =0 , y=0;
	
	initscr();
	
	while(1)
	{
		cls();
		refresh();
		switch( getch() ){
			case 'h': x--;break;
			case 'j' : y++; break;
			case 'k': y--;break;
			case 'l' : x++;break;
			
		}
		move( y,x );
		addch( 'a' );
	}
	
	refresh();
	getchar();
	endwin();
}
