

/*************************
  文字列のn文字目を取りだして詰める。
  元も文字列はエディットされる。
  値の範囲などは、上位ルーチンにお任せ。
*************************/
void shiftCharArray( char *src , int n )
{
	int i;
	for(i = n ; ;i++){
		if((src[i] = src[i+1])==0)return;
	}
}

void removeCRandLF( char *src )
{
	int i;
	for(i = 0 ; ; i++){
		if( src[i] == '\0')break;
		if( src[i] == 10 || src[i] == 13){
			shiftCharArray( src , i );
			i--;
		}
	}
}


main()
{
	char aho[100]={ 97,98,10,13,97,13,10,0};

	removeCRandLF( aho );
	printf(aho);
	
}
