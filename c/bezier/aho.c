



/*
  z1 と z2 の両方の端点に接するようなベジェ補完
  z をかえす。
  t は 0 と1の間である事。
  
 */
float bezier2_interpolation( float z1, float z2, float t )
{
    float z,zmid;
    float za,zb,zc;

    if( t <= 0.5f ){
        za = zb = z1;
        zc = (z1+z2)/2.0f;
        t *= 2.0f;
    }else {
        za = (z1+z2)/2.0f;
        zb = zc = z2;
        t = (t-0.5f)*2.0f;
    }
    return (1-t)*(1-t)*za + 2*(1-t)*t*zb + t*t*zc;                
}


void outout(float z )
{
    int i;
    for(i=0;i<50;i++){
        float q = (float)i / 50.0f;
        if( z > q ){
            printf( "*" );
        } else {
            printf( "-" );
        }
                   
    }

    printf( "\n" );
}

int
main()
{
    int i;
    int k = 50;

    for(i=0;i<k;i++){
        float z;
        z = bezier2_interpolation( 0.0f, 1.0f, (float)i / (float)k );
        outout(z);
    }
}
