struct k {
	int b[10];
	int a;
};

extern int a ;
extern int b;
int d;
extern int c;
extern struct k kk;

aho()
{
	
	printf("%d %d" , kk.a , kk.b[0]  );

}
