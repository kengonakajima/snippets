#include <stdio.h>


struct minfo
{
	int i;

	char aho[100];
	
};

struct minfo minfo;
struct minfo *pminfo;

void aho( struct minfo *pm , struct minfo m );

main()
{

	pminfo = (struct minfo *) malloc( sizeof( struct minfo ) );

	(*pminfo).i = 10;

	(&(*pminfo))->aho[0]= 0;

	printf( "%x %x %x %x\n" , pminfo , *pminfo ,&(*pminfo) , &pminfo );

	

	aho(  & (*pminfo ) , (*pminfo) );




}
void aho( struct minfo *pm , struct minfo m )
{
	pm->aho[0] = 120;
	m.aho[0] = 10;
	

	
}
