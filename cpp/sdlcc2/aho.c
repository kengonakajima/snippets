#include <stdio.h>

#include "objtypes.h"
#include "obj.tab.h"





meshes me;

int main( int c, char **v )
{
    FILE *fp;
    int r;
    char buf[10000];
    
    fp = fopen( v[1], "r");
    if(!fp)return -1;
    r = fread( buf, 1, sizeof(buf), fp );
    fclose(fp);

    
    
    r = obj_parse_meshes_in_buf( buf, r, &me );
    if(r<0){
        return 1;
    }
    obj_meshes_dump( &me );
    return 0;
}



