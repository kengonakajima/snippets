

/*************************
  $BJ8;zNs$N(Bn$BJ8;zL\$r<h$j$@$7$F5M$a$k!#(B
  $B85$bJ8;zNs$O%(%G%#%C%H$5$l$k!#(B
  $BCM$NHO0O$J$I$O!">e0L%k!<%A%s$K$*G$$;!#(B
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
