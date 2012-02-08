/*
  dijk.c : Get shortest route between two points, using dijkstra altorithm


  Creaeted by Kengo Nakajima (ringo@vce-lab.net) (2002July)

  This routine is originally for Luminousphere program

  
 */

/*
  
  Example: Input this graph, and START=A GOAL=G then A-B-F-G as answer

  
    A ---5----- B ---4----- C
    |           |\          |
    3           3 2         4
    |           |  \        |
    D ----4---- E-1-F----3--G
    
 */



#include <stdio.h>

#define DIJK_MAXP 200 /* max number of points */
typedef struct _dijk_t
{
    int start, goal; /* start point and goal point (starts with 1)*/
    float wt[DIJK_MAXP][DIJK_MAXP];       /* weights table  [i][j] */
} dijk_t;

void dijk_init( dijk_t *d);
void dijk_add_mutual_path( dijk_t *d, int i, int j, float cost );
void dijk_add_path( dijk_t *d, int i, int j, float cost );
void dijk_get_route( dijk_t *d, int s_i, int g_i, int *ans, int anslen,
                     float *cost );
void dijk_test(void);

int main()
{
    dijk_test();
}


enum{    A=1,B,C,D,E,F,G  }; /* A=1 G=7, see Example */

void dijk_test(void)
{
    dijk_t d;
    int ans[7],i;
    float cost;

    dijk_init( &d );
    dijk_add_mutual_path(&d, A,B,5.0f);
    dijk_add_mutual_path(&d, A,D,3.0f);
    dijk_add_mutual_path(&d, D,E,4.0f);
    dijk_add_mutual_path(&d, B,E,3.0f);
    dijk_add_mutual_path(&d, B,F,2.0f);
    dijk_add_mutual_path(&d, E,F,1.0f);
    dijk_add_mutual_path(&d, F,G,3.0f);
    dijk_add_mutual_path(&d, B,C,4.0f);
    dijk_add_mutual_path(&d, C,G,4.0f);
    dijk_get_route( &d, A,G, ans, 7, &cost );

    for(i=0;i<7;i++){
        printf( "Route %d = %d\n", i, ans[i] );
    }
    printf( "Cost %f\n", cost );
}

#undef TRUE
#define TRUE 1
#undef FALSE
#define FALSE 0
#define D_MAX (99999999999999999999999999999999999999.9f) /* arbitrary!! XXX*/



void dijk_init( dijk_t *d )
{
    int i,j;
    for(i=0;i<DIJK_MAXP;i++){
        for(j=0;j<DIJK_MAXP;j++){
            d->wt[i][j] = D_MAX;
        }
    }        
}
void dijk_add_mutual_path( dijk_t *d, int i, int j, float cost )
{
    dijk_add_path( d, i,j, cost );
    dijk_add_path( d, j,i, cost );
}
void dijk_add_path( dijk_t *d, int i, int j, float cost )
{
    d->wt[i][j] = cost;
}
void dijk_get_route( dijk_t *d, int s_i, int g_i, int *ans, int anslen,
                     float *cost )
{
    int visited[DIJK_MAXP+1];
    float distance[DIJK_MAXP+1];
    int prev[DIJK_MAXP+1];
    int i,j,next;
    float min;

    for(i=0;i<anslen;i++){
        ans[i] = -1;
    }

    for(i=1;i<=DIJK_MAXP;i++){
        visited[i] = FALSE;
        distance[i] = D_MAX;
    }
    distance[s_i] = 0;
    next = s_i;

    do {
        i = next;
        visited[i] = TRUE;
        min = D_MAX;

        for(j=1;j <= DIJK_MAXP; j++){
            if( visited[j] ) continue;
            if( ( d->wt[i][j] < D_MAX )
                && ( distance[i] + d->wt[i][j] < distance[j] ) ){
                distance[j] = distance[i] + d->wt[i][j];
                prev[j] = i;
            }
            if( distance[j] < min ){
                min = distance[j];
                next = j;
            }
        }
    } while( min < D_MAX );

#if 0    
    printf( "point nearp shortestd\n" );
    for(i = 1; i <= DIJK_MAXP; i++){
        if( i != s_i && visited[i]) {
            printf( "%2d%10d%10.1f\n", i, prev[i], distance[i] );
        }
    }
#endif
    {
        int cur = g_i;
        int index=0,k;

        ans[index++] = g_i;
        while(TRUE){
            int pr = prev[cur];

            ans[index++] = pr;
            cur = pr;
            if( pr == s_i ) break;
        }
    }
    *cost = distance[g_i];
}
