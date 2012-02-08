#include <malloc.h>
#include <stdio.h>

/* 3kb */
struct d
{

	int aho[100];
	char bohgeasdf[400];
	int hp;
	
	int dasfaho[100];
	char bodasfhgeasdf[400];
	
};
main()
{
	int charno = 3000;
	struct d* dd = (struct d*)malloc( sizeof(struct d)*charno);
	int i;
	
	if( dd==NULL){
		fprintf( stderr,"cufk");
		exit(0);
	}



	fprintf( stderr,"start");
	for(i=0;i<charno;i++){
		dd[i].hp ++;
	}
	
	fprintf( stderr,"end");
							   
}













