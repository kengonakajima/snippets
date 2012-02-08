
unsigned long clearBitLong( unsigned long src , long bitnum )
{
    unsigned long b = ( (unsigned long ) 0x1  << bitnum ) ^ (unsigned long)0xffffffff;

	return src & b;
}





int getTokenByCount( char *src , int num , char *out )
{
	int i;
	int count = 0;
	int start_i , end_i;
	int c=0,before_c=0;
	
	if( num <= 0 ) return -1;
	
	for(i=0;;i++){
		before_c = c;
		c = src[i];

		if( c != ' ' && c != '\t' && ( before_c == ' ' || before_c == '\t' )){
			count++;
			if( count == num ){
				int j, len;

				start_i = i;
				for(j = i; ;j++ ){
					c = src[j];
					if( c == ' ' || c == '\t' || c == '\0' || c ==  '\n' ){
						end_i = j-1;
						break;
					}
				}

				len = end_i - start_i + 1 ;
				strncpy( out , src + start_i , len );
				out[len] = '\0';

				return 0;
			}
		}
		if( c == '\0' ) break;
	}

	return -1;

}




main()
{
	char hoge[]=" 2303   345  67  	 987 abc ";
	char buf[100];

	int i;

	for(i=0;i<20;i++){
		int ret;
		ret = getTokenByCount( hoge , i , buf );
		printf( "i:%d ret:%d [%s]\n" , i, ret , buf );
	}

}


