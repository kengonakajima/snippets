int compareTop( char *longer , char *shorter )
{
	register int c;
	char *shorter_p = shorter;

	while( ( c = (*shorter++)) != '\0' ){
		if( c != (*longer++) )return 0;
	}

	/* $B:G8e$^$G$-$?$i!"D9$5$r7W;;$9$k$?$a$K0z$-;;$9$k(B */
	return (shorter-1) - shorter_p;
}




int main()
{
	printf( "%d\n" , compareTop( "hoggessadf" , "hogges" ) );
}
