/* TokenAdm.c */

#define TA

#include<stdio.h>
#include<string.h>
#include"TokenAdm.h"

void InitTT()
{
  tt.ctn=0;
}

int AddToken(char *p)
{
  int c;
  for(c=0;c<tt.ctn;c++){
    if(!strcmp(p,&tt.TokenTable[0][c]))return -1;
  }
  strcpy(&tt.TokenTable[tt.ctn][0],p);
  tt.ctn++;
  return 0;
}

int GetNumber(char *p)
{
  int c;
  for(c=0;c<tt.ctn;c++){
    if(!strcmp(p,&tt.TokenTable[c][0]))return c;
  }
  return -1;
}

char *GetChar(int i)
{
  return &tt.TokenTable[i][0];
}
