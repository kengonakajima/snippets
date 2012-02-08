void jEncode(char *src,int srclen,int key,char *encoded,int *encodedlen,int maxencodedlen)

{

	char sum;
	int i;
	if(srclen+1 > maxencodedlen){
		
		/* $B%(%s%3!<%I8e$OD9$5$,(B1$B$N$S$k!#%^%C%/%9$r$3$($?$i%(%s%3!<%I$;$:C1$K%3%T!<$9$k$@$1(B*/

		*encodedlen = maxencodedlen;
		for(i=0;i<(*encodedlen);i++){
			encoded[i] = src[i];
		}
	}

	if(srclen+1 <= maxencodedlen){

		/*$B%^%C%/%9$3$($J$$$H$-(B */
		
		*encodedlen=srclen+1;
		for(i=0;i<srclen;i++){
			sum = sum + src[i];
            printf( "Sum:%d\n" , sum );
            
			/*$B@hF,$+$i(B7$B$NG\?t8DL\$+(B2$B$NG\?t8DL\$N$H$3$m$O%S%C%HH?E>(B*/
			
			if(((key%7) == (i%5))||((key%2) == (i%2))){
				src[i] = ~src[i];
			}
            printf( "src[%d]=%d\n", i , src[i] );
		}

		for(i=0;i<(*encodedlen);i++){
			if(abs((key%srclen)) > i){			
                encoded[i] = src[i] + sum*((i*i)%3);	/*$B$F$-$H$&$K$?$7$F$k(B*/
			}

			/*key%srclen$B$N0LCV$KA4%S%C%H$r$?$7$?$b$N$rCV$/(B*/
			
			if(abs((key%srclen)) == i){
				encoded[i] = sum;
			}
			if(abs((key%srclen)) < i){
				encoded[i] = src[i-1] + sum*((i*i)%7);	/*$B$F$-$H$&$K$?$7$F$k(B*/
			}

            printf( "encoded[%d]=%d\n", i,encoded[i]&0xff);
		}

	}

}
void jDecode(char *src,int srclen,int key,char *decoded,int *decodedlen)

{

	/*$B%(%s%3!<%I$N5U(B*/
	char sum;
	int i;
	*decodedlen=srclen-1;
	sum = src[abs(key%(*decodedlen))];

	for(i=0;i<srclen;i++){
		if(abs((key%(*decodedlen))) > i){
			decoded[i] = src[i] - sum*((i*i)%3);
		}

		if(abs((key%(*decodedlen))) < i){
			decoded[i-1] = src[i] - sum*((i*i)%7);
		}
	}
	for(i=0;i<(*decodedlen);i++){
		if(((key%7) == (i%5))||((key%2) == (i%2))){
			decoded[i] = ~decoded[i];
		}
	}


}

main()
{
    char hoge[100],encoded[111];
    int elen;
    
    hoge[0] = 100;
    hoge[1] = 200;
    hoge[2] = 30;

    jEncode( hoge , 3,893, encoded , &elen, sizeof(encoded)    );

    printf( "%d %d %d %d\n" ,encoded[0] , encoded[1] , encoded[2] ,encoded[3]);
    
}






