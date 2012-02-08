#include<stdio.h>
#include<stdlib.h>
#include"TokenAdm.h"
#include"DbAdm.h"
#include"Query.h"

main(int argc,char *argv[])
{
  FILE *fp;
  int i;
  fprintf( stderr ,"%d\n",argc);
  if(argc==2){
    fp=fopen(argv[1],"r");
    if(fp==NULL){
      fprintf(stderr , "Error to open File");
      exit(1);
    }
    ReadDataBase(fp);
  }else {
      fprintf(stderr ,"No Input File Specifyed.");
      exit(1);
  }
	printf("read was finished\n\n");
  printf("%s\n",Qrandom());
	printf("%s\n",Query("random"));
  
  
/*  printf( "%s\n" , Qrandom());
  printf(......., Query("random"));
  prinf(........, Query("weapon&powerful"));*/
}

