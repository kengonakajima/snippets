

#define W 10
#define H 10

int ary[] =
{
    0,0,0,0,0,0,0,0,0,0,
    0,1,1,0,0,0,0,0,0,0,
    0,0,1,0,0,1,1,1,1,1,
    0,0,1,1,0,1,1,1,1,1,
    0,0,1,1,1,1,0,0,1,0,
    0,0,0,0,0,0,0,0,1,0,
    0,0,0,0,1,1,1,1,1,0,
    0,1,1,0,1,0,0,0,0,0,
    0,0,0,0,1,0,0,0,0,1,
    0,0,0,0,1,1,1,1,1,1,    
};

int wk[W*H];


/*
  step1: �ǽ�ˤߤĤ���1��2���ѹ����롥
  step2: �����򥹥���󤷡�2�ΤȤʤ��1��2���ѹ����Ƥ�����
         ���꤫���������2�ο��������ʤ��ʤä���롼��ȴ����
  step3: �ޤ�1�����ä��� 0���֤����ʤ��ä���1���֤�
*/
   
int check_connected( int *a, int w, int h )
{
    int cnt,x,y,done,prevcnt;

    /* step 1 */
    done=0;
    for(y=0;y<h;y++){
        for(x=0;x<w;x++){
            if( a[x+y*w] == 1 ){
                a[x+y*w] = 2;
                done=1;
                break;
            }
        }
        if(done)break;
    }

    /* step 2 */
    cnt = 1;
    for(;;){
        prevcnt = cnt;
        for(y=0;y<h;y++){
            for(x=0;x<w;x++){
                int k;
                k = a[x+y*w];
                if( k == 2 ){
                    /* change left */
                    if(x>0 && a[(x-1)+y*w] == 1){
                        a[(x-1)+y*w] = 2;
                        cnt++;
                    }
                    /* change right */
                    if(x<(w-1) && a[(x+1)+y*w] == 1 ){
                        a[(x+1)+y*w] = 2;
                        cnt++;
                    }
                    /* change top */
                    if(y>0 && a[x+(y-1)*w] == 1 ){
                        a[x+(y-1)*w] = 2;
                        cnt++;
                    }
                    /* change bottom */
                    if(y<(h-1) && a[x+(y+1)*w] == 1){
                        a[x+(y+1)*w] = 2;
                        cnt++;
                    }
                }
            }
        }
        for(y=0;y<h;y++){
            for(x=0;x<w;x++){
                printf( "%d,", a[x+y*w] );
            }
            printf("\n");
        }
        printf("\n");
        if( prevcnt == cnt ) break;
    }

    /* step 3 */
    for(y=0;y<h;y++){
        for(x=0;x<w;x++){
            if( a[x + y*w] == 1 ){
                return 0;
            }
        }
    }
    return 1;
}

int main( int argc, char **argv )
{
    memset( wk, 0, sizeof(wk));

    printf( "%d\n",
            check_connected( ary, W, H ) );

}

