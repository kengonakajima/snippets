int jEncode(char *src,int srclen,int key,char *encoded,int *encodedlen,int maxencodedlen)
{
	char sum;
	int i;
	if(srclen+1 > maxencodedlen){
		*encodedlen = maxencodedlen;
		for(i=0;i<(*encodedlen);i++) encoded[i] = src[i];
	}
	if(srclen+1 <= maxencodedlen){
		*encodedlen=srclen+1;
		for(i=0;i<srclen;i++){
			sum = sum + src[i];
			if(((key%7) == (i%5))||((key%2) == (i%2)))src[i] = ~src[i];
		}
		for(i=0;i<(*encodedlen);i++){
			if(abs((key%srclen)) > i) encoded[i] = src[i] + sum*((i*i)%3);
			if(abs((key%srclen)) == i) encoded[i] = sum;
			if(abs((key%srclen)) < i) encoded[i] = src[i-1] + sum*((i*i)%7);
		}
        return 0;
	} else {
        return -1;
    }
}
void jDecode(char *src,int srclen,int key,char *decoded,int *decodedlen)
{
	char sum;
	int i;
	*decodedlen=srclen-1;
	sum = src[abs(key%(*decodedlen))];
	for(i=0;i<srclen;i++){
		if(abs((key%(*decodedlen))) > i) decoded[i] = src[i] - sum*((i*i)%3);
		if(abs((key%(*decodedlen))) < i) decoded[i-1] = src[i] - sum*((i*i)%7);
	}
	for(i=0;i<(*decodedlen);i++){
		if(((key%7) == (i%5))||((key%2) == (i%2))) decoded[i] = ~decoded[i];
	}
}








