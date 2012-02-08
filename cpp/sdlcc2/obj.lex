%{

#include "objtypes.h"
#include "obj.tab.h"
#include <string.h>
    
extern char *obj_buf;
extern int obj_sz, obj_so_far;
extern int obj_num_lines;

 

#define yywrap() 1
#define YY_INPUT(buf,res,max) \
{\
    int cplen,rest;\
    rest = obj_sz - obj_so_far;\
    if( max > rest ){\
        cplen = rest;\
        if( cplen == 0 ){\
            res = 0;\
        }\
    } else {\
        cplen = max;\
    }\
    if( cplen > 0 ){\
      memcpy( buf, obj_buf + obj_so_far, cplen );\
      res = cplen;\
      obj_so_far += res;\
    }\
}
    

 int obj_blaces_num ;


 
%}

%s MESH_STATE

%%

[ \t]  { ; }
\n     { obj_num_lines ++; }

Mesh   {
    BEGIN MESH_STATE;

    if( obj_meshes->m_num == MAX_MESHES ){
        fprintf(stderr,"too many meshes\n");
        return 0;
    }
    obj_cur_mesh = (mesh*) malloc( sizeof(mesh));
    if( !obj_cur_mesh ){
        fprintf(stderr,"malloc faile\n");
        return 0;
    }
    memset( obj_cur_mesh, 0, sizeof(mesh));
    obj_meshes->mp[obj_meshes->m_num] = obj_cur_mesh;
    obj_blaces_num = 0;    
    return MESH;
}

<MESH_STATE>MeshMaterialList { return MESHMATERIALLIST; }
<MESH_STATE>Material { return MATERIAL; }
<MESH_STATE>TextureFilename { return TEXTUREFILENAME; }
<MESH_STATE>MeshNormals { return MESHNORMALS; }
<MESH_STATE>MeshTextureCoords { return MESHTEXTURECOORDS; }


<MESH_STATE>[a-zA-Z_][a-zA-Z0-9_.]* { snprintf( yylval.sval,sizeof(yylval.sval), "%s", yytext ); return NAME; }
<MESH_STATE>-?[0-9]+"."[0-9]+ { yylval.fval = atof(yytext); return FLOAT; }
<MESH_STATE>-?[0-9]+   { yylval.ival = atoi(yytext); return INTEGER; }

<MESH_STATE>"{"     {
    /*fprintf( stderr, "OPEN\n");*/
    obj_blaces_num ++;
    return '{';
}

<MESH_STATE>"}"     {
    /* fprintf(stderr, "CLOSE\n");*/
    obj_blaces_num --;
    if( obj_blaces_num > 0 ){
        return '}';
    } else if( obj_blaces_num == 0 ){
        BEGIN 0;
        return '}';
    } else {
        ;
    }
    }
<MESH_STATE>.      {
    return yytext[0];
}


. {;}

%%



/*
<MESH_STATE>\"[a-zA-Z0-9_.]+\" { snprintf( yylval.sval,sizeof(yylval.sval), "%s", yytext); return STRING; }
*/
