int compressRunLength( int *src , int src_len , int *dest , int *dest_len ,int dest_maxlen)
{
	int i,c=0,destc=0;

	for(i=0;i<src_len;i++){

		c++;
		dest[destc] = src[i];
		dest[destc+1] = c;

		if( src[i] != src[i+1] ){
			c = 0;
			destc+=2;
			if( destc >= dest_maxlen ){
				return -1;
			}
			
			
		} else
		if( i == ( src_len -1 ) ){
			destc += 2;
			break;
		}
	}

	*dest_len = destc;
	return 0;

}


main()
{

	int hoge[10]={0,0,0,0,0,0,0,0,0,0};
	int dest[1000],dlen;
	
	printf( "ret: %d\n" , compressRunLength( hoge, 10 , dest , &dlen , 1000 ) );
	printf( "[%d] %d %d  %d %d %d %d %d %d %d %d %d %d %d \n" ,
		   dlen , 
		   dest[0] ,
		   dest[1] ,
		   dest[2] ,
		   dest[3] ,
		   dest[4] ,
		   dest[5] ,
		   dest[6] ,
		   dest[7] ,
		   dest[8] ,
		   dest[9] ,
		   dest[10]  );
	
}


