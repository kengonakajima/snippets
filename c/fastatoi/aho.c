#include <stdio.h>
#include <sys/time.h>

/*
  PenPro 200 : Cereron 400 は4倍。 ループ込み。
  atoi を 100万回/sec
  strlen を  500万回/sec
  空ループは 1億回/sec
  aaaa を 400万回/sec

  hatoi は 1000万回
 */

/*
  自前でつくると、 inline にできるから、ふつうのatoiより速い。
 */
static inline int
fastatoi( char *s , int l )
{
    int minus = 1;

#define DIF(d) ( s[d] - '0' )    
    if( s[0] == '-' ){ minus = -1; l--; s++;}

    switch(l){
    case 0: return 0;
    case 1: return (DIF(0)) * minus;
    case 2: return (DIF(0)*10+DIF(1))*minus; 
    case 3: return (DIF(0)*100+DIF(1)*10+DIF(2))*minus;
    case 4: return (DIF(0)*1000+DIF(1)*100+DIF(2)*10+DIF(3))*minus;
    case 5: return (DIF(0)*10000+DIF(1)*1000+DIF(2)*100
                    +DIF(3)*10+DIF(4))*minus;

    case 6: return (DIF(0)*100000+DIF(1)*10000+DIF(2)*1000+DIF(3)*100
                    +DIF(4)*10+DIF(5))*minus;
    case 7: return (DIF(0)*1000000+DIF(1)*100000+DIF(2)*10000
                    +DIF(3)*1000+DIF(4)*100+DIF(5)*10+DIF(6))*minus;
    case 8: return (DIF(0)*10000000+DIF(1)*1000000+DIF(2)*100000
                    +DIF(3)*10000+DIF(4)*1000+DIF(5)*100
                    +DIF(6)*10+DIF(7))*minus;
    case 9: return (DIF(0)*100000000+DIF(1)*10000000+DIF(2)*1000000
                    +DIF(3)*100000+DIF(4)*10000+DIF(5)*1000
                    +DIF(6)*100+DIF(7)*10+DIF(8))*minus;
    case 10: return (DIF(0)*1000000000+DIF(1)*100000000+DIF(2)*10000000
                    +DIF(3)*1000000+DIF(4)*100000+DIF(5)*10000
                    +DIF(6)*1000+DIF(7)*100+DIF(8)*10+DIF(9))*minus;
    default:
        printf( "asdfa\n" );
        return 0;
    }
}

/* 上のをswitchだけでやってみた */
#define SWSW(a,mul) switch(a){case '0':break; case '1':v+=1*mul;break; case '2':v+=2*mul;break; case '3':v+=3*mul;break; case '4':v+=4*mul;break; case '5':v+=5*mul;break; case '6':v+=6*mul;break; case '7':v+=7*mul;break; case '8':v+=8*mul;break; case '9':v+=9*mul;break; }

static inline int
atoi5( char *s, int l )
{
    int v=0;

    switch(l){
    case 0: return 0;
    case 1: return DIF(0);
    case 2: SWSW(s[0],10); v+=DIF(1); return v;
    case 3: SWSW(s[0],100); SWSW(s[1],10); v+=DIF(2); return v;
    case 4: SWSW(s[0],1000); SWSW(s[1],100); SWSW(s[2],10);
        v+=DIF(3);
        return v;
    case 5: SWSW(s[0],10000);SWSW(s[1],1000); SWSW(s[2],100); SWSW(s[3],10); v+=DIF(4); return v;

    }
}

static inline int
hatoi( char *s, int l )
{
#define HXI(c) ( (c) >= 'a' ? ( 10+(c)-'a') : (c)-'0')

    switch(l){
    case 0: return 0;
    case 1: return HXI(s[0]);
    case 2: return (HXI(s[0])<<4) + (HXI(s[1]));
    case 3: return (HXI(s[0])<<8) + (HXI(s[1])<<4) + HXI(s[2]);
    case 4: return (HXI(s[0])<<12) + (HXI(s[1])<<8) + (HXI(s[2])<<4) +HXI(s[3]);
    case 5: return (HXI(s[0])<<16)+(HXI(s[1])<<12)+(HXI(s[2])<<8)+(HXI(s[3])<<4)+HXI(s[4]);
    case 6: return (HXI(s[0])<<20)+(HXI(s[1])<<16)+(HXI(s[2])<<12)+(HXI(s[3])<<8)+(HXI(s[4])<<4)+HXI(s[5]);
    case 7: return (HXI(s[0])<<24)+(HXI(s[1])<<20)+(HXI(s[2])<<16)+(HXI(s[3])<<12)+(HXI(s[4])<<8)+(HXI(s[5])<<4)+HXI(s[6]);
    case 7: return (HXI(s[0])<<28)+(HXI(s[1])<<24)+(HXI(s[2])<<20)+(HXI(s[3])<<16)+(HXI(s[4])<<12)+(HXI(s[5])<<8)+(HXI(s[6])<<4)+HXI(s[7]);
    default:return 0;
    }
}

int
main()
{
    int i;
    struct timeval t, et;
    char string[] = "2007";
    int aa;

    
    gettimeofday( &t, NULL );

    aa = strlen( string );
    for(i=0;i<10000000;i++){
         fuckfunc( hatoi( string, aa ));
    //    printf( "[%x] \n", hatoi( string, aa ));
    }
    gettimeofday( &et, NULL );

    printf( "%d\n", ( et.tv_sec - t.tv_sec ) * 1000000 +
            ( et.tv_usec - t.tv_usec ));
    return 0;
    
}
