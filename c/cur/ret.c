#include <stdio.h>
#include <curses.h>


main()
{
	int i;

	initscr();

	while(1){
		
		i = getch();
		printf("%d\n" , i );
	}
}

	
