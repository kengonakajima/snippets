int hashCode( char *buf , int len , int *table )
{
	int i,j,k;

	int blen;
	int v=0;
	
	blen = strlen(buf );
	
	for(i = 0 ; i < blen ; i++){
		int x;

		x = buf[i]&0xf;
		v += (x << ( i%32));
	}

	if( len == 0 ){
		return v;
	}
	if( len > 0 ){
		for(i=0;i<len;i++){
			if( table[i]==v){
				for(i=1;i <2100000000;i++){
					for(j =0;j<len;j++){
						if( i == table[j] )break;
					}
					if( j == len ){
						/* $BI=$K$J$$$b$N$@$C$?!#(B */
						return i;
					}
				}
				return 0;	/* 21$B2/$NCf$K$O$J$$!#$d$P$$$h!#(B */
			}
		}
		return v;
	}
	if( v == 0 ){
		for(i=1;i <2100000000;i++){
			for(j =0;j<len;j++){
				if( i == table[j] )break;
			}
			if( j == len ){
				/* $BI=$K$J$$$b$N$@$C$?!#(B */
				return i;
			}
		}
		return 0;	/* 21$B2/$NCf$K$O$J$$!#$d$P$$$h!#(B */
	}



	return 0;
}

int main()
{
	int t[10]={116,1,4,5,6,7,8,9,10 ,340};
	
	printf( "%d\n" , hashCode( "fuck_shit_movf_fuck." ,10,t ));
	printf( "%d\n" , hashCode( "fuck_shit_move_fuck." ,10,t ));
	
}
