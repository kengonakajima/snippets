#include <stdio.h>

int d = 1050;
int c = 2000;
int b = 2800;
int a = 5000;


#if 1
int ticket_num = 7063;
int total_yen = 9083800;
#endif

//int ticket_num = 5410;
//int total_yen = 7323250;



int main()
{
    int an;
//    for(an=ticket_num;an>=0;an--){
    for(an=30;an>=30;an++){
        int t = an * a;
        int bn;
        fprintf( stderr, ".");

        
        for(bn=ticket_num;bn>=0;bn--){
            int tt = t + bn * b;
            int cn;
            for(cn=ticket_num;cn>=0;cn--){
                int dn;
                int ttt = tt + cn * c;
                if( ttt > total_yen )continue;

                if( ((total_yen - ttt) % d )== 0 ){
                    dn = (total_yen-ttt)/d;
                    if( (an+bn+cn+dn) == ticket_num){
                        fprintf( stderr,
                                 "%d*%d + %d*%d + %d*%d + %d*%d = %d, %d\n",
                                 a,an,
                                 b,bn,
                                 c,cn,
                                 d,dn,
                                 a*an+b*bn+c*cn+d*dn,
                                 an+bn+cn+dn
                            );
                    }
                }
            }
        }
    }
}

