#define Q

#include <stdio.h>
#include <string.h>
#include "TokenAdm.h"
#include "DbAdm.h"
#include "Query.h"

char *Qrandom(void)
{
	int x,y,c;
	aq.num=0;
	for(x=0;x<ITEMS;x++){
		for(y=0;y<DATAS;y++){
			if(dd[y].d[x]<0){
				aq.ans[aq.num]=-1-dd[y].d[x];
				aq.num++;
			}
		}
	}
	c=random() % aq.num;
	return GetChar(aq.ans[c]);
}

char *Query(char *p)
{
	if(!strcmp(p,"random"))return Qrandom();
	return "Unknown Query!";
}
