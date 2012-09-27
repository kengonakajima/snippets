#include <math.h>
#include <stdio.h>

int vecToDir( float x, float y) {
    const float pi4 = M_PI / 4.0f;
    float at = atan2( x,y );
        fprintf( stderr, "at:%f\n", at );
    if( at >= -pi4  && at <= pi4 ){
        return 0; // up
    } else if( at >= pi4 && at <= pi4*3){
        return 1; // r
    } else if( at >= pi4*3 || at <= -pi4*3 ){
        return 2; // d
    } else if( at <= -pi4 && at >= -pi4*3 ){
        return 3; // l
    } else {
        return 0;
    }
        
}
int main(){

    fprintf(stderr, "R 10,0: %d\n", vecToDir(10,0) );
    fprintf(stderr, "UR 10,10: %d\n", vecToDir(10,10) );
    fprintf(stderr, "U 0,10: %d\n", vecToDir(0,10) );
    fprintf(stderr, "UL -10,10: %d\n", vecToDir(-10,10) );
    fprintf(stderr, "L -10,0: %d\n", vecToDir(-10,0) );
    fprintf(stderr, "DL -10,-10: %d\n", vecToDir(-10,-10) );
    fprintf(stderr, "D 0,-10: %d\n", vecToDir(0,-10) );
    fprintf(stderr, "DR 10,-10: %d\n", vecToDir(10,-10) );                        

}
