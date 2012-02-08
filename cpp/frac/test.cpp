#include <iostream>

#define SZ 100

class Rect
{
 public:
    int x1,y1,x2,y2;

    // 0 1
    // 2 3
    Rect *child[4]; 
    
    // do not include x2,y2
    Rect(int x1, int y1, int x2, int y2 ){
        //        std::cout << "new: " << x1 << "," << y1 << "," << x2 << "," << y2 << std::endl;
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        this->child[0] = this->child[1] = this->child[2] = this->child[3] = 0;
    }

    // return a rect that hits
    Rect *get( int x, int y ){
        for(int i=0;i<4;i++){
            if( child[i] &&
                x >= child[i]->x1 &&
                y >= child[i]->y1 &&
                x < child[i]->x2 &&
                y < child[i]->y2 ){            
                return child[i]->get(x,y);
            }
        }
        if( x >= x1 && y >= y1 && x < x2 && y < y2 ){
            return this;
        }
    }
    
    // return false if no more division
    bool divide( int x, int y){
        if( x2 == x1 && y2 == y1 ){ // minimum!
            return false;
        }
        
        for(int i=0;i<4;i++){
            if( child[i] &&
                x >= child[i]->x1 &&
                y >= child[i]->y1 &&
                x < child[i]->x2 &&
                y < child[i]->y2 ){            
                return child[i]->divide(x,y);
            }
        }


        if( x >= x1 && y >= y1 && x < x1 + ( x2 - x1 )/2 &&  y < y1 + ( y2 - y1 )/2 ){
            child[0] = new Rect( x1,
                                 y1,
                                 x1 + ( x2 - x1 )/2 ,
                                 y1 + ( y2 - y1 )/2 );
        }
        if( x >= x1 + ( x2 - x1 )/2 && y >= y1 && x < x2 && y < y1 + ( y2 - y1 )/2 ){
            child[1] = new Rect( x1 + ( x2 - x1 )/2 ,
                                 y1,
                                 x2,
                                 y1 + ( y2 - y1 )/2 );
        }
        if( x >= x1 && y >= y1 + ( y2 - y1 )/2 && x < x1 + ( x2 - x1 )/2 && y < y2 ){
            child[2] = new Rect( x1,
                                 y1 + ( y2 - y1 )/2,
                                 x1 + ( x2 - x1 )/2 ,
                                 y2 );
        }
        if( x >= x1 + ( x2 - x1 )/2 && y >= y1 + ( y2 - y1 )/2 && x < x2 && y < y2 ){
            child[3] = new Rect( x1 + ( x2 - x1 )/2 ,
                                 y1 + ( y2 - y1 )/2,
                                 x2,
                                 y2 );
        }
        return true;
    }
    void set( int*ary, int w, int h ){
        for(int i=0;i<4;i++){
            if(child[i]){
                child[i]->set(ary,w,h);
            }
        }
        
        ary[ (x1+x2)/2 + (y1+y2)/2 * w ] += 1;
    }
    
};

int main()
{
    Rect *r = new Rect(0,0,SZ,SZ);

    int cnt = 0;
    for(int i=0;i<100;i++){
        int x = rand() % SZ;
        int y = rand() % SZ;

        Rect *hr;
        hr = r->get( x,y );
        if( hr && hr->size() < ( rand() % SZ )){
            hr->divide(x,y);
        }
    }

    int ary[SZ*SZ];
    memset( ary, 0, sizeof(ary));
    r->set(ary,SZ,SZ);

    for(int y=0;y<SZ;y++){
        for(int x=0;x<SZ;x++){
            if( ary[x+SZ*y] ){
                std::cout << "*";
            } else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}
