void jEncode(char *src,int srclen,int key,char *encoded,int *encodedlen,int maxencodedlen)

{

	char sum;
	int i;
	if(srclen+1 > maxencodedlen){
		
		/* エンコード後は長さが1のびる。マックスをこえたらエンコードせず単にコピーするだけ*/

		*encodedlen = maxencodedlen;
		for(i=0;i<(*encodedlen);i++){
			encoded[i] = src[i];
		}
	}

	if(srclen+1 <= maxencodedlen){

		/*マックスこえないとき */
		
		*encodedlen=srclen+1;
		for(i=0;i<srclen;i++){
			sum = sum + src[i];
            printf( "Sum:%d\n" , sum );
            
			/*先頭から7の倍数個目か2の倍数個目のところはビット反転*/
			
			if(((key%7) == (i%5))||((key%2) == (i%2))){
				src[i] = ~src[i];
			}
            printf( "src[%d]=%d\n", i , src[i] );
		}

		for(i=0;i<(*encodedlen);i++){
			if(abs((key%srclen)) > i){			
                encoded[i] = src[i] + sum*((i*i)%3);	/*てきとうにたしてる*/
			}

			/*key%srclenの位置に全ビットをたしたものを置く*/
			
			if(abs((key%srclen)) == i){
				encoded[i] = sum;
			}
			if(abs((key%srclen)) < i){
				encoded[i] = src[i-1] + sum*((i*i)%7);	/*てきとうにたしてる*/
			}

            printf( "encoded[%d]=%d\n", i,encoded[i]&0xff);
		}

	}

}
void jDecode(char *src,int srclen,int key,char *decoded,int *decodedlen)

{

	/*エンコードの逆*/
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






