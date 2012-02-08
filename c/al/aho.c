struct aho
{
	int amem;
	int bmem;
	char cmem;
	double dmem;
	
};
struct aho a[100];

struct aho *hoge(void)
{
	return &a[1];
}
main()
{
	a[1].dmem=1.2;
	
	printf( "%f" , hoge()->dmem );
}

