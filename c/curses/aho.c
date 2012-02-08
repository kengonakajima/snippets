#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>


#define SIZE 64

typedef enum {
    AIR = 0,
    LEAF = 1,
    STEM = 2,
    ROOT = 3,
    SOIL = 4,
    ROCK = 5,
    WATER = 6,
} CellType;


CellType data[SIZE][SIZE];

void sim();

int main(void)
{
    /* スクリーンの初期化 */
    if (initscr() == NULL) {
        fprintf(stderr, "error init screen\n");
        exit(-1);
    }

    sim();
    
    /* ウィンドウを閉じる */
    if (endwin() == ERR) {
        fprintf(stderr, "error end win\n");
        exit(-2);
    }
    
    return 0;
}

void initField() {
    for(int y=0;y<SIZE;y++){
        for(int x=0;x<SIZE;x++){
            if( y > (SIZE/2) ){
                data[y][x] = AIR;
            } else {
                data[y][x] = SOIL;
            }
        }
    }
}

void updateField() {
    for(int y=0;y<SIZE;y++){
        for(int x=0;x<SIZE;x++){
            char c;
            switch(data[y][x] ){
            case AIR:break;
            case LEAF: c = '*'; break;
            case STEM: c = '+'; break;
            case ROOT: c = '$'; break;
            case SOIL: c = '.'; break;
            case ROCK: c = '#'; break;
            case WATER: c = '~'; break;
            default:
                c = "?";
            }
            mvprintw( y,x, "%c", c );
        }
    }
}

void sim() {

    initField();

    cbreak();
    
    while(1){
        /* 画面クリア */
        clear();
    

        updateField();
        refresh();
        usleep(100*1000);

        ch = getch();
        if(ch == EOF)break;
    }    
}

