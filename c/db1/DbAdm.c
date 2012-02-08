#define DA

#include<string.h>
#include<stdio.h>
#include"TokenAdm.h"
#include"DbAdm.h"

void ReadLine(FILE *,int);
int getToken(FILE *,int,int);
int z;

int ReadDataBase(FILE *fp)
{
  char c;
  int d,i;
  d=1;
  i=0;
    c=fgetc(fp);	  
  while(d){
    if(c!='%'){
	    fprintf(stderr, "%c",c);
	    d=0;
	    break;
    }	    
    fprintf(stderr,"Start a Line %d \n", i);
    ReadLine(fp,i);
    c=fgetc(fp);
    i++;
  }
  return 0;
}

void ReadLine(FILE *fp,int i)
{
	char c;
	int d,f,x;
	d=1;
	x=0;
	z=0;
	while(d){
		f=getToken(fp,x,i);
		fprintf( stderr , "gotToken %d %d %d %d\n",i,x,f,z);
		x++;
		if (f<0)d=0;
	}
}
		

int getToken(FILE *fp,int x,int y)
{
	/* if the token is "..." return 1,else 0 */
	int i,p,r;
	int d,f;
	char c;
	char buf[StringLength];
	d=1;
	p=0;f=0;
	if(z==1){
		z=0;
		r=1;
	}else r=0;
	while(d){
		c=fgetc(fp);
		fprintf(stderr ,"%c",c);
		if(c=='\n')return -3;
		if(c!=' ' && c!=',' && c!='\"')d=0;
		if(c=='\"')f=1;
	}
	d=1;
	while(d){
		if(c!=',' && c!='\"'){
			buf[p]=c;
			p++;
			c=fgetc(fp);
			if(c=='\"')z=1;
			if(c=='\n')return -2;
		} else {
			buf[p]=0;
			d=0;
		}
	}
	if(r){
		f=1;
		r=0;
	}
	AddToken(buf);
	dd[y].d[x]=GetNumber(buf);
	fprintf( stderr , ",%s\n" ,GetChar(dd[y].d[x]));
	if(f==1)dd[y].d[x]=-1-dd[y].d[x];
	return f;
}
		


















