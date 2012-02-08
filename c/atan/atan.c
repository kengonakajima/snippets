#include <math.h>

#define PI ( 3.1415926535f)


float calc_z_angle( float x, float y )
{
    float dy;
    
    if( x == 0.0f && y > 0 ) return PI*0.5f;
    if( x == 0.0f && y < 0 ) return PI*1.5f;
    if( x == 0.0f && y == 0.0f ) return 0.0f; // arbitrary..

    dy = (float)fabs(y/x);

    if( y > 0.0f ){
        return (float)atan(dy);
    } else {
        return (float)(PI - atan(dy));
    }
}

int main()
{
    float t;
    t = calc_z_angle( 1.0f, 1.0f );
    printf(" %f\n",t);
}
