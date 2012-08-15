struct Point
{
    float x,y;
};




export uniform int hitcnt( uniform Point points[], uniform int n ) {
    int hitcnt=0;
    for( uniform int index=0; index<n; index++){
        Point p = points[index];
        print("% % %\n", index, p.x, p.y );
        int ishit=0;
        foreach(j=0 ... n ) {
            float<2> tgtpos = { points[j].x, points[j].y };
            float d=4;
            if( p.x+d > tgtpos[0]-d && p.x-d < tgtpos[0]+d && p.y+d > tgtpos[1]-d && p.y-d < tgtpos[1]+d ) {
                ishit=1;
                // breakしたいがエラーになるのだ。
            }
        }
        if(ishit==1){
            hitcnt++;
        }
    }
    return reduce_add(hitcnt);
}
