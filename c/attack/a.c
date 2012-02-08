#include <stdio.h>


#define RAND() ( random() % 32768 )




int calcDamage( int totaldef , int wstr , int str )
{
	float r;
	int damage = 0;
	float totalstr;
	float deft;

	totalstr = ((float)str*  RAND() )/65536.0 + ( str/2) +  (float)wstr;

	if( totaldef < 0 ){
		totalstr += (-totaldef*4);
		totaldef = 0;
	}
	if( totalstr == 0 ) totalstr = 1;
	if( totaldef == 0 ) totaldef = 1;

	deft = totalstr + totaldef*10;

	{
		r =  ( totalstr / deft ) ;
		damage = totalstr * r * (  0.75 + ((float)RAND())/(65536.0*2.0));
	}
	
	if( damage < 0) damage =  0; 

	return damage;
}



int main(int argc , char **argv)
{
	int i;

	for(i=0;i<40;i++){
		printf( "%d\n" , calcDamage( atoi( argv[1] ), atoi( argv[2] ), atoi( argv[3] ) ) );
	}
	
}


