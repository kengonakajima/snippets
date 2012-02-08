

main()
{
	while(1)
	{

		char *buf;
		getchar();
		
		buf = (char *)malloc(1000000);
		free(buf);
	}
}
