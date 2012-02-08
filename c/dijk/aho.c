
/*
  Get nearest way by dijkstra
  
 */

#define START 1
#define NMAX 7

/*
    A ---5----- B ---4----- C
    |           |\          |
    3           3 2         4
    |           |  \        |
    D ----4---- E-1-F----3--G
    
 */
float weight[NMAX][NMAX];

enum
{
    A=1,B,C,D,E,F,G
};
#define SET(i,j,v) { weight[i][j] = weight[j][i] = v; }
#define TRUE 1
#define FALSE 0
#define D_MAX 9999999999.9f

int initweight(void)
{
    int i,j;
    for(i=0;i<NMAX;i++){
        for(j=0;j<NMAX;j++){
            weight[i][j]= D_MAX;
        }
    }
    SET(A,B,5.0f);
    SET(A,D,3.0f);
    SET(D,E,4.0f);
    SET(B,E,3.0f);
    SET(B,F,2.0f);
    SET(E,F,1.0f);
    SET(F,G,3.0f);
    SET(B,C,4.0f);
    SET(C,G,4.0f);
};

int main( int argc, char **argv )
{
    int i,j,next;
    float min;

    char visited[NMAX+1];
    float distance[NMAX+1];
    int prev[NMAX+1];

    initweight();

    for(i=1;i<=NMAX;i++){
        visited[i] = FALSE;
        distance[i] = D_MAX;
    }
#define START A

    distance[START] = 0;
    next = START;

    do {
        i = next;
        visited[i] = TRUE;
        min = D_MAX;

        for(j=1;j <= NMAX; j++){
            if( visited[j] ) continue;
            if( ( weight[i][j] < D_MAX )
                && ( distance[i] + weight[i][j] < distance[j] ) ){
                distance[j] = distance[i] + weight[i][j];
                prev[j] = i;
            }
            if( distance[j] < min ){
                min = distance[j];
                next = j;
            }
        }
    } while( min < D_MAX );

    printf( "point nearp shortestd\n" );
    for(i = 1; i <= NMAX; i++){
        if( i != START && visited[i]) {
            printf( "%2d%10d%10.1f\n", i, prev[i], distance[i] );
        }
    }
    /* Let's start making route */
#define GOAL G

    {
        int cur = GOAL;
        int route[NMAX+1];
        int index=0,k;

        route[index++] = GOAL;
        while(TRUE){
            int pr = prev[cur];

            route[index++] = pr;
            cur = pr;
            if( pr == START ) break;
        }
        for(k=0;k<index;k++){
            printf( "Route %d = %d\n", k, route[k] );
        }
    }

    
    return 0;
}
    
