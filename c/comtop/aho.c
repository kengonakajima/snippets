int compareTop( char *longer , char *shorter )
{
	register int c;
	char *shorter_p = shorter;

	while( ( c = (*shorter++)) != '\0' ){
		if( c != (*longer++) )return 0;
	}

	/* 最後まできたら、長さを計算するために引き算する */
	return (shorter-1) - shorter_p;
}




int main()
{
	printf( "%d\n" , compareTop( "hoggessadf" , "hogges" ) );
}
