#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <iostream>

#define HEIGHT 50
#define WIDTH 120

typedef enum {
    NOVALUE=0,
    LIGHTAIR,
    DARKAIR,
    LEAF,
    NEWLEAF,
    WATEREDSTEM,          // 水有りの幹
    DRYSTEM,              // 水無し
    WATEREDROOT,          // 水有りの根
    DRYROOT,              // 水無しのね
    SOIL,
    SAND,
    ROCK,
    WATER,
    CELLMAX
} CellType;


CellType data[HEIGHT][WIDTH];


class Pos {
 public:
    int x,y;
    Pos(int _x,int _y){ x=_x; y=_y;}
    Pos(){ x=y=0;}

    bool isValid(){
        if( x== -1 && y == -1)return false; else return true;
    }
    void setInvalid() {
        x = -1; y = -1;
    }
};

#define RANDOM() (random() / 8 )

void setd(int x,int y, CellType t ){
    data[y][x] = t;
}
CellType getd(int x,int y){
    if( x<0||y<0|| x>=WIDTH||y>=HEIGHT){return NOVALUE;}
    return data[y][x];
}





void initField() {
    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++){
            if( y > (HEIGHT/2) ){
                setd(x,y,LIGHTAIR);
            } else {
                setd(x,y,SOIL);
            }
        }
    }
    setd( WIDTH/2,HEIGHT/2+1,WATEREDSTEM);
    setd( WIDTH/2,HEIGHT/2,WATEREDROOT);
}


char toChar( CellType t ){
    char c;
    switch(t){
    case LIGHTAIR: c = ' '; break;
    case DARKAIR: c = '|'; break;
    case LEAF: c = '*'; break;
    case NEWLEAF: c = '+'; break;
    case WATEREDSTEM: c = '@'; break;
    case DRYSTEM: c = 'O'; break;        
    case WATEREDROOT: c = '$'; break;
    case DRYROOT: c = 'S'; break;
    case SOIL: c = ':'; break;
    case SAND: c = '.'; break;        
    case ROCK: c = '#'; break;
    case WATER: c = '~'; break;
    default:
        c = '?';
    }
    return c;
}




// 近くに1個でもあったらtrue
int near( int minx, int miny, int maxx, int maxy, CellType t ){
    int c=0;
    for( int xx=minx; xx <= maxx; xx++){
        for(int yy=miny; yy <= maxy; yy++){
            if( getd(xx,yy)==t){
                c++;
            }
        }
    }
    return c;
}

// TODO: 左上が優先される. to randomise!
int findNear( int minx, int miny, int maxx, int maxy, CellType t, int *outx, int *outy ){
    int c=0;
    for( int xx=minx; xx <= maxx; xx++){
        for(int yy=miny; yy <= maxy; yy++){
            if( getd(xx,yy)==t){
                *outx = xx;
                *outy = yy;
                c++;
            }
        }
    }
    return c;
}
Pos getNearestPos( int minx, int miny, int maxx, int maxy, CellType t ) {
    Pos cand[1000];
    int c=0;
    for( int xx=minx; xx <= maxx; xx++){
        for(int yy=miny; yy <= maxy; yy++){
            if( getd(xx,yy)==t){
                cand[c++] = Pos(xx,yy);
            }
        }
    }
    if(c==0){
        return Pos(-1,-1);
    } else {
        return cand[RANDOM()%c];
    }

}



// そのセルが太陽光に当たってるならtrue
bool isLit( int x, int y ){
    return near( x-3, y,x+3,y+2, LIGHTAIR);
}

// Xの差で2,yの差で3以内に幹があるならtrue
int isNearWateredStem( int x, int y ){
    return near( x-2, y-3, x+2, y, WATEREDSTEM );
}
int countNearAnyStem( int minx, int miny, int maxx, int maxy ) {
    return near(minx,miny,maxx,maxy,WATEREDSTEM) + near(minx,miny,maxx,maxy,DRYSTEM);
}
int isNearAnyStem( int x, int y ){
    return countNearAnyStem( x-2, y-3, x+2, y );
}



// Xの差で1以内に葉か幹があるならOK
bool isNearLeaf( int x, int y ){
    return near( x-1, y-1, x+1, y, LEAF );
}

// x,y: 新しい葉っぱの位置
bool findSetExtendableStem( int x, int y ){
    int esx, esy;
    if( findNear( x-3, y-4, x+3, y-1, WATEREDSTEM, &esx, &esy ) ){
        int putx, puty;
        // xの差が1か0か-1だったら上に、それ以外だったら横に
        switch( x-esx){
        case 0:
        case 1: // 真上かそのとなり
        case -1:
        case -2:
        case 2:
            putx = esx; puty = esy + 1;
            break;
            //        case 2:
        case 3:
            putx = esx+1; puty = esy+1;
            if( (RANDOM()%100)<40)return false;
            //            if( (RANDOM()%100)<90) puty = esy;
            break;
            //        case -2:
        case -3:
            putx = esx-1; puty = esy+1;
            if( (RANDOM()%100)<40)return false;
            //            if( (RANDOM()%100)<90) puty = esy;            
            break;
        default:
            std::cerr << "xy:" << x << "," << y << " esxy:" << esx << "," << esy << std::endl;
            abort();
        }

        // まわりが幹がたくさんあるなら幹をつくらない。
        // 幹の流れは別
        int nwstem = near( putx-1, puty-1, putx+1,puty+1,WATEREDSTEM);
        int ndstem = near( putx-1, puty-1, putx+1,puty+1,DRYSTEM);
        
        if( (nwstem+ndstem) < 4 ){
            setd( putx, puty, DRYSTEM );
            setd( putx, puty-1, DRYSTEM );            
        }
        return true;
    } else {
        return false;
    }
}

// 新しい葉っぱ
void doNewLeaf(int x, int y ){
    if( isNearWateredStem( x,y ) ){
        setd( x,y, LEAF );
        // 新芽が葉っぱに成長するときに水を消費して幹を乾かす
        Pos wspos = getNearestPos( x-2,y-3,x+2,y, WATEREDSTEM );
        if( wspos.isValid() ){
            setd( wspos.x, wspos.y, DRYSTEM );
        }        
    } else {
        // 条件があわない
        // 幹を作れるなら作る
        if( !findSetExtendableStem(x,y) ){
            setd( x,y, DARKAIR );
        }
    }

}

// 葉っぱ　増えようとする
void doLeaf( int x, int y ) {


    // 日に当たってないなら死ぬ
    if( !isLit(  x,y ) ){
        setd(x,y,DARKAIR);
        return;
    }
    
    // 支える幹がないなら死ぬ
    if( !isNearAnyStem( x,y ) ){
        setd(x,y,DARKAIR);
        return;
    }
        // 幹があっても水がないときが続いたら死ぬ
    if( ( !isNearWateredStem(x,y) ) &&
        ( (RANDOM() % 100) < 10 ) ){
        setd(x,y,DARKAIR);
        return;
    }

    // 条件が揃っているなら新芽を出す
    // まずOK位置をもとめる
    int minx = x - 1;
    int miny = y;
    int maxx = x + 1;
    int maxy = y + 1;

    Pos okpos[5*5];
    int oknum=0;
    
    for( int nx = minx; nx <=  maxx; nx ++ ){
        for(int ny = miny; ny <= maxy; ny ++ ){
            CellType t = getd(nx,ny);
            if( t == LIGHTAIR ){
                okpos[oknum++] = Pos(nx,ny);
            }
        }
    }

    if( oknum>0){
        // okposから選ぶ
        Pos p = okpos[ RANDOM() % oknum ];
        setd( p.x, p.y, NEWLEAF );
        
    }
    
    
}

// 木の幹の動き
void doWateredStem( int x, int y ) {

    // 太陽にあたってるなら葉っぱを出す
    if( isLit(x,y) ){
        int lx = x;
        int ly = y+1;
        if( getd(lx,ly)==LIGHTAIR){
            setd(lx,ly,LEAF);
            setd(x,y,DRYSTEM);
        }
    }

#if 0
    // 幹の流れ
    // 一段したの列を見て、 -3 ~ +3 の間に幹がない場合、 -4か4のところにあったらそこに幹を移動する
    // 水が足りなくなったら移動するのほうがいいかもな。
    if( getd(x,y-1) != SOIL ){ // 地面の上にあるのは死なない
        if( near( x-3, y-1,x+3,y-1, WATEREDSTEM) == 0 ){
            Pos okpos[2], putpos[2];
            int oknum=0;
            if( getd( x-4, y-1 ) == STEM ){
                okpos[oknum] = Pos(x-4,y-1);
                putpos[oknum] = Pos(x-3,y-1);
                oknum++;
            }
            if( getd( x+4, y-1 ) == STEM ){
                okpos[oknum] = Pos(x+4,y-1);
                putpos[oknum] = Pos(x+3,y-1);
                oknum++;
            }
            if( oknum>0){
                Pos p = putpos[ RANDOM() % oknum ];
                setd( p.x, p.y,DARKAIR);
                std::cerr << "went down! "  << std::endl;
            }
        }
    }
#endif
    
}

int g_stemupflow=0; // 幹が下から上に水を上げた階数
int g_rootin=0; // 根が水を吸い込んだ回数

void doDryStem( int x, int y ) {
    
    // 水を下の幹か根から吸い上げる
    if( getd( x,y-1) == WATEREDSTEM ){
        setd(x,y,WATEREDSTEM);
        setd(x,y-1,DRYSTEM);
        return;
    }
    if( getd(x,y-1) == WATEREDROOT ){
        setd(x,y,WATEREDSTEM);
        setd(x,y-1,DRYROOT );
        g_stemupflow++;
        return;
    }

    // 下にない場合は左か右の幹から.根っこは真下からのみ
    Pos wateredpos[2];
    int wnum=0;
    if(getd(x-1,y) == WATEREDSTEM ){
        wateredpos[wnum]=Pos(x-1,y);
        wnum++;
    }
    if(getd(x+1,y) == WATEREDSTEM ){
        wateredpos[wnum]=Pos(x+1,y);
        wnum++;
    }
    if(wnum>0){
        int i = RANDOM() % wnum;
        setd(x,y,WATEREDSTEM);
        setd( wateredpos[i].x, wateredpos[i].y, DRYSTEM );
        return;
    }

    // もし下が土だったら根をはる
    if( getd(x,y-1)==SOIL){
        setd(x,y-1,DRYROOT);
    }

}

// 光は下45度方向に広がる
void doLightAir( int x , int y ){
    if( getd( x, y-1) == DARKAIR){
        setd(x, y-1, LIGHTAIR );
    }
    if( getd( x-1, y-1) == DARKAIR){
        setd(x-1, y-1, LIGHTAIR );
    }
    if( getd( x+1, y-1) == DARKAIR){
        setd(x+1, y-1, LIGHTAIR );
    }
}

// 根っこ
void doDryRoot( int x, int y ){
    // 下と左右から水を吸う
    if( getd(x,y-1) == SOIL ){
        setd(x,y-1,SAND);
        setd(x,y,WATEREDROOT);
        g_rootin++;
        if( ( RANDOM() % 100 ) < 50 ) {
            setd(x,y-1,DRYROOT);
        }
        return;
    }
    if( getd(x-1,y) == SOIL ){
        setd(x-1,y,SAND);
        setd(x,y,WATEREDROOT);
        g_rootin++;
        return;
    }
    if( getd(x+1,y) == SOIL ){
        setd(x+1,y, SAND);
        setd(x,y,WATEREDROOT);
        g_rootin++;
        return;
    }
    // 下が根っこだったときも吸う
    if( getd(x,y-1)== WATEREDROOT){
        setd(x,y-1,DRYROOT);
        setd(x,y,WATEREDROOT);
        return;
    }
        
    
}
void doWateredRoot( int x, int y ) {
    
}

// 砂は下から水を吸い上げる
void doSand( int x, int y ) {
    if( getd(x,y-1)==SOIL){
        setd(x,y,SOIL);
        setd(x,y-1,SAND);
    }
        
}
// 土は45度下に水を移動する
void doSoil( int x, int y ) {
    Pos topos[3];
    int n=0;
    if( getd(x,y-1)==SAND){
        topos[n++]=Pos(x,y-1);
    }
    if( getd(x-1,y-1)==SAND){
        topos[n++]=Pos(x-1,y-1);
    }
    if( getd(x+1,y-1)==SAND){
        topos[n++]=Pos(x+1,y-1);
    }
    if(n>0){
        Pos fixed = topos[ RANDOM() % n ];
        setd(x,y,SAND);
        setd( fixed.x, fixed.y,SOIL);
    }

}

// セルを動かす
void moveCell( int x, int y ) {
    CellType t = getd(x,y);
    
    switch(t){
    case LIGHTAIR: doLightAir(x,y); break;
    case DARKAIR:break;
    case LEAF:doLeaf(x,y);break;
    case NEWLEAF: doNewLeaf(x,y); break;
    case WATEREDSTEM:doWateredStem(x,y);break;
    case DRYSTEM:doDryStem(x,y);break;
    case WATEREDROOT:doWateredRoot(x,y);break;
    case DRYROOT:doDryRoot(x,y);break;
    case SAND:doSand(x,y);break;
    case SOIL:doSoil(x,y);break;
    case ROCK:break;
    case WATER:break;
    default:break;
    }
}

// 動かす
void simulate(int c) {

    if((c%2)==0){
        for(int y=0;y<HEIGHT;y++){
            for(int x=0;x<WIDTH;x++){
                if( getd(x,y)==NEWLEAF){
                    moveCell(x,y);
                }
            }
        }
    } else {
        for(int y=0;y<HEIGHT;y++){
            for(int x=0;x<WIDTH;x++){
                if( getd(x,y)!=NEWLEAF){
                    moveCell(x,y);
                }
            }
        }
#if 0        
        for(int y=0;y<HEIGHT;y++){
            for(int x=1;x<WIDTH;x+=2){
                if( getd(x,y)!=NEWLEAF){
                    moveCell(x,y);
                }
            }
        }
#endif        
    }
}


void updateDisplay() {
    static int cnt=0;

    std::cerr << cnt <<std::endl;
    cnt++;

    int tcnt[CELLMAX];
    memset(tcnt,0,sizeof(tcnt));
    
    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++){
            CellType t = getd(x,HEIGHT-y-1);
            char c = toChar(t);
            std::cerr << c;
            tcnt[t]++;
        }
        std::cerr << std::endl;
    }
    // stat
    for(int i=0;i<CELLMAX;i++){
        std::cerr << toChar( (CellType)i) << "=" << tcnt[i] << " ";
    }
    std::cerr << std::endl;
    std::cerr <<"stemup:" << g_stemupflow << " rootin:" << g_rootin << std::endl;
}


int main(void)
{
    srandom(time(NULL));
    initField();

    int c=0;
    while(1){
        updateDisplay();
        int ch = fgetc(stdin);
        if(ch==EOF)break;

        c++;
        g_stemupflow=0;
        g_rootin=0;
        simulate(c);
        
    }    
    
    return 0;
}
