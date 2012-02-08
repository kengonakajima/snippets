#include <stdio.h>
#include <stdlib.h>


int main()
{
	fprintf( stderr , "%f\n" , 32.9);
	fprintf( stderr , "%f\n" , atof("32.9"));
	fprintf( stderr , "%f\n" , atof("32.9;"));
	fprintf( stderr , "%f\n" , atof("32.9%"));    
   	fprintf( stderr , "%f\n" , atof("32.9 "));
   	fprintf( stderr , "%f\n" , atof(" 32.9\n"));            
}
