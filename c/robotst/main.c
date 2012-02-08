#define MAIN_C
#include <time.h>
#include <sys/time.h>

#include "general.h"

void do_auto();

void main(int argc , char  *argv[])
{
	int i,j;

	fd_set readfds;
	struct timeval tv;
	char buf[256];
	char buf2[256];
	int nfds;
	int n;
	int tmp;
	FILE *toringo;
	
	/* default  setting*/

	game.xsize = 59;
	game.ysize = 22;

	

	game.trial  = 1;
	game.start_robots = 40;
	game.start_junks = 0;

	if(argc >= 2) game.do_auto = 1;
	if(argc >=3) game.do_disp = 0; else game.do_disp = 1;		
	/* initialize */
	
	initscr();

	noecho();
	nonl();

	raw();
	cbreak();
	
	while(1)
	{

		if(game.do_auto){
			/* 自動実行の時は、ここを起動 */
			FD_ZERO(&readfds);
			FD_SET(0,&readfds);
			nfds  = select(1,&readfds,NULL,NULL,&tv);
			if(nfds ){
				if( FD_ISSET(0,&readfds) ){
					if(n = read(0,buf,sizeof(buf)) ){
						buf[n] = '\0';
					} else {
						buf[0] = '\0';
					}
				}
			} else {
				buf[0] = '\0';
			}
			if(buf[0]=='q') {
				endwin();
				printf("\n");
				exit(-1);
			}
			
		}
		
		time(& game.current_time);
		if(game.current_robots == 0){
			newmap();
			game.stage ++;
			time( &game.start_time); 
			srand( (unsigned) game.start_time);		
		}
		
		init_screen(game.xsize,game.ysize);

		game.turn++;
		if(game.do_auto) do_auto(); else

		switch(getch())
		{
			case 'q' :{
				erase();
				endwin();
				printf("\n");
				exit(0);
				break;
			}
			case 'a' :	{
						if(game.teleportmode){
							game.teleportmode = 0;
						} else {
							game.teleportmode = 1;
						}
						break;
					}
			
			case 'h' :	walk(-1,0,WALK);	break;
			case 'j' :	walk(0,1,WALK);	break;
			case 'k' :	walk(0,-1,WALK);	break;
			case 'l' :	walk(1,0,WALK);	break;
			case 'b' :	walk(-1,1,WALK);	break;
			case 'n' :	walk(1,1,WALK);	break;
			case 'y' :	walk(-1,-1,WALK);	break;
			case 'u' :	walk(1,-1,WALK);	break;
			case ' ' :	walk(0,0,WALK); break;
			case 't' :	teleport(); break;
			case 'w' :	{
					wait();
					break;
				}
			case 'i' :	  do_auto();
				
			default : break;
		
					
		}


	}

	
}
void  do_auto()
{
	int i;
	
   i = think(game.ret);
   game.teleported = 0;
   switch(i){
	   case 1:game.ret =walk(-1,1,WALK);break;
	   case 2:game.ret = walk(0,1,WALK);break;
	   case 3:game.ret = walk(1,1,WALK); break;
	   case 4:game.ret = walk(-1,0,WALK); break;
	   case 6:game.ret =walk(1,0,WALK); break;
	   case 7:game.ret = walk(-1,-1,WALK);break;
	   case 8:game.ret = walk(0,-1,WALK);break;
	   case 9:game.ret = walk(1,-1,WALK);break;
	   case 0:game.ret = walk(0,0,WALK); break;
	   case 5:game.ret = 0;teleport();game.teleported = 1; break;
	   case 10: game.ret = 0;wait(); break;
	   default: break;
   }
}
void teleport()
{
	int i,j;
	
	i = RAND()*(game.xsize+1) / 32768;
	j = RAND()*(game.ysize+1) / 32768;
	walk(i-game.mx,j-game.my,TELE);
}
#define WAIT 3

void wait()
{
	int i;
	int robots;
	robots = game.current_robots;

	for(i=0;i<57;i++){
		if(walk(0,0,WAIT) == -1){
			die();
			return;
		};
		init_screen(game.xsize,game.ysize);
		if(game.current_robots == 0){
			/* cleared. */
			game.score +=robots;
			return;
		}
	}
	
}


int walk(int rx,int ry,int mode)
{
	int x,y;
	int i,j;
	int tmprx[ROBOMAX];
	int tmpry[ROBOMAX];
	int relx,rely;
	int mxstore,mystore;
	int scorestore;

	
	
	/* 動く予定の場所 */
	
	x = game.mx+rx;
	y = game.my+ry;

	if(x<0 || y<0 || x>game.xsize || y>game.ysize){
		/* 音だす */

		return -1;
	}
	if(game.mapdata[x][y] == JUNK){
		return -1;
	}
	
	/* テンポラリの地形をクリア */
	for(i = 0 ; i < game.xsize+1 ; i++){
		for(j = 0  ; j < game.ysize+1 ; j++){
			game.tmpmap[i][j] = FLOOR;
			
			if(game.mapdata[i][j] == JUNK) game.tmpmap[i][j] = JUNK;
			
		}
	}

	game.tmpmap[x][y] = ME;
	
	scorestore  = game.score;
	/* テンポラリの地形に、次の地形を作る。 */
	
	for(i = 0 ; i< game.xsize +1 ; i++){
		for(j = 0 ; j < game.ysize+1; j++){
			
			if(game.mapdata[i][j]==ROBOT){
				relx = 0;
				rely = 0;
				if( i>x) relx = -1;
				if( i<x) relx = 1;
				if( j>y) rely = -1;
				if( j<y) rely = 1;
				switch(game.tmpmap[i+relx][j+rely]){
					case ROBOT : game.score+=20;
						game.tmpmap[i+relx][j+rely] = JUNK; break;
					case JUNK :  ; game.score+=10;break;
					case ME : {
						if(mode == TELE){
							game.score = scorestore;
							die();
						}
						game.score  = scorestore;
						
						return -1;
					}
					
					case FLOOR : game.tmpmap[i+relx][j+rely] = ROBOT; break;
					default : break;
				}
				
					
			}
		}
	}
	for(i = 0 ; i < game.xsize+1; i++){
		for(j = 0 ; j < game.ysize +1 ; j++){
			game.mapdata[i][j] = game.tmpmap[i][j];
		}
	}
	/*歩けた。 */
	game.mx = x;
	game.my = y;
	game.score  = scorestore;
	
	return 0;
}
void die()
{
	if(game.do_disp){
		
	move(game.my + 1,game.mx+1);
	printw("Agagagagagagaga!!!!!");
	refresh();
	if(game.do_auto ==0){
		getch();
	}
}
	
	if(game.score > game.hscore[0]) game.hscore[0] = game.score;

	game.totalscore +=game.score;
	
	game.score = 0;
	game.turn = 0;
	game.trial ++;
	game.stage = 0;
	game.ret = RET_DIE;	

	
	newmap();
}

void newmap()
{
	int i,j,k,l;


	int robots = 0 ,junks = 0 ;
	refresh();
	for(i = 0 ; i < game.xsize  +1;i++){
		for(j  = 0 ; j < game.ysize+1 ; j++){
			game.mapdata[i][j] = FLOOR;
		}

	}	
	/* make robots */
	
	for(l = 0  ; l< 10000 ; l++){
		i = RAND()*(game.xsize+1) / 32768;
		j = RAND()*(game.ysize+1) / 32768;
		
		if(game.mapdata[i][j] == FLOOR){
			game.mapdata[i][j] = ROBOT;
			robots++;
			if(robots == game.start_robots) break;
		} 
		
	}

	/* make junks */
	if(game.start_junks !=0){
	for(l = 0  ; l < 10000 ; l++){
		i = RAND()*(1+game.xsize) / 32768;
		j = RAND()*(1+game.ysize) / 32768;
		if(game.mapdata[i][j] == FLOOR){
			game.mapdata[i][j] = JUNK;
			junks++;
			if(junks == game.start_junks) break;
		}
		
	}
	}
	/* make me */
	for(l = 0  ; l < 10000 ; l++){
		i = RAND()*(game.xsize+1) / 32768;
		j = RAND()*(game.ysize+1) / 32768;
		if(game.mapdata[i][j] == FLOOR){
			game.mx = i;
			game.my = j;
			game.mapdata[i][j] = ME;
			break;
		}
		
	}

	
}

void init_screen(int xsize,int ysize)
{

	int i,j,k,c;
	
	xsize+=2;
	ysize+=2;
if(game.do_disp) {
	erase();


	move(0,0);
	printw("+");
	move(ysize,0);
	printw("+");
	move(0,xsize);
	printw("+");
	move(ysize , xsize);
	printw("+");

	for(i = 1 ; i < xsize ;i++) { move(0,i);printw("-"); }
	for(i = 1 ; i < xsize ;i++) { move(ysize,i);printw("-"); }
	for(i = 1 ; i < ysize ;i++) { move(i,0);printw("|"); }
	for(i = 1 ; i < ysize ;i++) { move(i,xsize);printw("|"); }

	move(ysize+ 1 ,0);
	printw("robo2 hjklyubn.. move t..teleport w..wait iI..auto solver ");
	move(ysize+ 2 ,0);
	printw("a ..auto teleport on/off s..score write (-j option.. junk bonus on)");
	
}
	game.current_robots = 0;		
	for(i = 0 ; i < game.ysize +1; i++){

		for(j = 0 ; j  < game.xsize+1 ; j++){
			if(game.do_disp){
				move(i+1,j+1);
				addch(game.mapdata[j][i]);
			}
			if(game.mapdata[j][i] == ROBOT){
				game.current_robots ++;
			}
		}
	}

	if(game.do_disp){
		move(game.my+1,game.mx+1);
		addch(ME);
	}	

	if(game.current_robots == 0 || game.do_disp){

		move(ysize +4,0);
		printw("score %d(%d)[%f] : robots %d junks %d turn %d stage %d  time %d  TRY %d  ",
	       game.score,game.hscore[0],
		game.trial ==0 ? 0 : (float)game.totalscore / (float) (game.trial-1),
	       game.current_robots,
	       game.current_junks,
	       game.turn,
	       game.stage,
		game.current_time - game.start_time ,
		game.trial
	       );
	}

	if(game.do_disp) refresh();	
	
	
}


