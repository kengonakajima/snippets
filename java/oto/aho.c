#include <stdio.h>
 
main()
{
  register int i=0;

  fprintf(stderr,"asdfasdf\n");
  for(i = 0 ; i < 200000000 ; i++){
	if( i == 100000000)break;
  }
  fprintf(stderr,"asdfasdf\n");
}
