struct program
{
	int c;
	
	char name[234];
	int ind;
	
};

main()
{
	printf("%d\n" , sizeof( (struct program *)->name));
}


