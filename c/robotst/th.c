#define TH_C
#include "general.h"



int think(int );


int c;
int phase = 0;

int dir[8] = {
	6,4,2,8
};

int pret = -1;

int mem[100][100];
int robotx[50];
int roboty[60];
int robon = 0;
int junkx[50];
int junky[50];
int junkn;

int gx,gy;	/* 重心 */

int roboread();

int spacemode;	/* 止まったらスペースを押し続ける */

int rdir[10]={	/* テンキーを0-7方向に変換 */
	0,5,4,3,6,0,2,7,0,1
};
int _rdir[8]={	/* 0-7方向をテンキーに変換 */
	8,9,6,3,2,1,4,7
};
int think(int ret)
{
	int i;
	i = think_sub(ret);

	return i;
}

int think_sub(int ret)
{
	roboread();

	getg(&gx,&gy);

	if(ret == RET_CANTWALK && spacemode == 0 && game.teleported == 0){
		spacemode = 1;
		return SPACE;
	}
	
	if(ret == RET_CANTWALK && spacemode == 1){
		spacemode = 0;
		
		return TELEPORT;
	}
	
	if(spacemode){
		return SPACE;

	}
	if( (game.mx<9) || (game.my > (game.xsize-9)) || (game.my<4) || (game.my>(game.ysize-4))){
		
		return _rdir[(4+rdir[dir8(game.mx,game.my,gx,gy)]) & 7];
	} else {
		return dir8(game.mx,game.my,gx,gy);
	}
	return SPACE;
}

int getg(int *gx,int *gy){
	int i;
	int x=0,y=0;
	
	for(i = 0 ; i < robon ; i++){
		x+=robotx[i];
		y+=roboty[i];

	}

	if(robon!=0){
		*gx = x / robon;
		*gy = y / robon;
	} 
}

int roboread()
{
	int i;
	int j;

	robon = 0;
	junkn = 0;
	
	for(i = 0 ; i <= game.xsize  ;i++){
		for(j = 0; j<= game.ysize ; j++){
			if(game.mapdata[i][j]==ROBOT){
				
				robotx[robon] = i;
				roboty[robon] = j;
				
				robon++;
			}
			if(game.mapdata[i][j] == JUNK){
				junkx[junkn] = i;
				junky[junkn] = j;
				junkn++;
			}
			
		}
	}
}

int dir8(int x,int y,int dx,int dy)
{
	if(x==dx){
		if(y<dy) return 2;
		if(y>dy) return 8;
		if(y==dy) return 1;
	} else if(x > dx){
		
		if(y<dy) return 1;
		if(y>dy) return 7;
		if(y==dy) return 4;
	} else {
		if(y<dy) return 3;
		if(y>dy) return 9;
		if(y==dy) return 6;
	}
}
