
	int countChar( char *buf , int c ,int n)
	{
		        int i;
			        int count = 0;
			        for(i = 0 ; ;i++){
					                if( buf[i] == '\0' ) return -1;
							                if( buf[i] == c ){
										                        count++;
													                        if( count == n ) return i;
												}
						}

			        return -1;
		}

	
main()
{
	char buf[] = "bokasd,asdf,asdf,sd";
	printf("%s\n" , buf + countChar( buf , ',' , 2 ));
}








