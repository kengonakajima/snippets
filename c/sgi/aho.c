#include <stdio.h>

main()
{
  FILE *fp;
int i = 0xaa112233;
fp=fopen("ahoboke","w");
fwrite( &i , 4 , 1, fp );

  
}
