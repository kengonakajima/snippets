%{



#include "objtypes.h"

#include <string.h>
#include <stdlib.h>

#define ERROR_RETURN return 1;
    
void yyerror( char *);    
int yylex(void);


meshes *obj_meshes;
char *obj_buf;
int obj_sz, obj_so_far;
int obj_num_lines = 0;
mesh *obj_cur_mesh;


%}


%union {
    float fval;
    int ival;
    char sval[1024];
    rgbacolor rgbaval;
    rgbcolor rgbval;
    faceinds faceindsval;
    vector3d vec3val;
    vector2d vec2val;
    vectors3d vecs3val;
    vectors2d vecs2val;
    face faceval;
    faces facesval;
    vertex vertexval;
    vertices verticesval;
    texturefile texturefileval;
    material materialval;
    materials materialsval;
}

%token <sval> NAME 
%token <fval> FLOAT
%token <ival> INTEGER

%token MESH MATERIAL TEXTUREFILENAME MESHNORMALS MESHTEXTURECOORDS
%token MESHMATERIALLIST

%type <rgbaval> rgbacolor
%type <rgbval> rgbcolor
%type <faceindsval> faceinds
%type <vec3val> vector3d
%type <vec2val> vector2d
%type <vecs3val> vectors3d
%type <vecs2val> vectors2d
%type <faceval> face
%type <facesval> faces
%type <vertexval> vertex
%type <verticesval> vertices
%type <texturefileval> texturefile
%type <materialval> material
%type <materialsval> materials

%%

meshes: meshes mesh 
| mesh 
; 





mesh: MESH NAME '{' INTEGER ';' vertices ';' INTEGER ';' faces ';' mesh_opts '}' {
    extern mesh *obj_cur_mesh;
    memcpy( (char*)&obj_cur_mesh->v, & $6, sizeof( vertices ));
    memcpy( (char*)&obj_cur_mesh->f, & $10, sizeof( faces ));
    if( $6.v_num != $4 ){
        yyerror( "bad v_num\n");
        ERROR_RETURN;
    }
    if( $10.f_num != $8 ){
        yyerror( "bad f_num\n");
        ERROR_RETURN;        
    }
    obj_meshes->m_num ++;
}
 ;

mesh_opts: mesh_opts mesh_opt
| mesh_opt
;

mesh_opt: meshmateriallist {;}
| meshtexturecoords {;}
| meshnormals {;}
;


faces: faces ',' face {
    if( $$.f_num == MAX_FACES ){
        yyerror( "too many faces\n");
        ERROR_RETURN;                
    } else {
        $$.f[$$.f_num] = $3;
        $$.f_num ++;
    }
}
| face { $$.f[0] = $1; $$.f_num = 1; }
 ;

face: INTEGER ';' INTEGER ',' INTEGER ',' INTEGER ';' {
/*    fprintf( stderr, "face3: %d,%d,%d\n",$3,$5,$7);*/
    $$.vinds[0] = $3;
    $$.vinds[1] = $5;
    $$.vinds[2] = $7;
    $$.vinds_num = 3;
}
| INTEGER ';' INTEGER ',' INTEGER ',' INTEGER ',' INTEGER ';' {
/*    fprintf( stderr, "face4 %d,%d,%d,%d\n",$3,$5,$7,$9);*/
    $$.vinds[0] = $3;
    $$.vinds[1] = $5;
    $$.vinds[2] = $7;
    $$.vinds[3] = $9;
    $$.vinds_num = 4;
}
 ;


vertices:  vertex { $$.v[0] = $1;  $$.v_num = 1;}
| vertices ',' vertex  {
    if( $$.v_num == MAX_VERTICES ){
        yyerror( "too many vertex\n");
        ERROR_RETURN;                        
    } else {
        $$.v[ $$.v_num ] = $3;
        $$.v_num ++;
    }
}
 ;

vertex: FLOAT ';' FLOAT ';' FLOAT ';'      {
    $$.x = $1;
    $$.y = $3;
    $$.z = $5;
/*    fprintf( stderr, "vertex:(%f,%f,%f)\n", $1, $3, $5 );*/
}
 ;





materials: materials material {
    if( $$.mat_num == MAX_MATERIAL ){
        yyerror( "too many material\n");
        ERROR_RETURN;                                
    } else {
        $$.mat[ $$.mat_num ] = $2;
        $$.mat_num ++;
    }
}
| material { $$.mat[0] = $1; $$.mat_num = 1; }
 ;

material: MATERIAL '{' rgbacolor ';' FLOAT ';' rgbcolor ';' rgbcolor ';' texturefile '}' {
    $$.facecol = $3;
    $$.power = $5;
    $$.specular = $7;
    $$.emissive = $9;
    $$.texf = $11;
}
 | MATERIAL '{' rgbacolor ';' FLOAT ';' rgbcolor ';' rgbcolor ';' '}'
{
    $$.facecol = $3;
    $$.power = $5;
    $$.specular = $7;
    $$.emissive = $9;
    $$.texf.filename[0] = 0;
}
 ;

rgbacolor: FLOAT ';' FLOAT ';' FLOAT ';' FLOAT ';' {
    $$.r = $1;
    $$.g = $3;
    $$.b = $5;
    $$.a = $7;
}
 ;

rgbcolor: FLOAT ';' FLOAT ';' FLOAT ';' { $$.r = $1; $$.g = $3; $$.b = $5; }
 ;

texturefile: TEXTUREFILENAME '{' '"' NAME '"' ';' '}'   {
    snprintf( $$.filename,sizeof( $$.filename),"%s", $4 );
}
 ;


meshmateriallist: MESHMATERIALLIST '{' INTEGER ';' INTEGER ';' faceinds ';' materials '}' {
    extern mesh *obj_cur_mesh;
    obj_cur_mesh->ml.matlist = $9;
    obj_cur_mesh->ml.finds = $7;
}
 ;


faceinds: faceinds ',' INTEGER {
    if( $$.inds_num == MAX_FACEINDS ){
        yyerror( "too many faceind\n");
        ERROR_RETURN;                                
    } else {
        $$.inds[ $$.inds_num ] = $3;
        $$.inds_num ++;
    }
}
| INTEGER { $$.inds[0] = $1; $$.inds_num = 1; }
 ;


meshnormals: MESHNORMALS '{' INTEGER ';' vectors3d ';' INTEGER ';' faces ';' '}'
{
    extern mesh *obj_cur_mesh;
    if( $5.v_num != $3 ){
        yyerror( "bad v_num\n");
        ERROR_RETURN;                                
    }
    if( $9.f_num != $7 ){
        yyerror( "bad f_num\n");
        ERROR_RETURN;                                
    }
    obj_cur_mesh->mn.v = $5;
    obj_cur_mesh->mn.f = $9;
}
 ;


vectors3d: vectors3d ',' vector3d {
    if( $$.v_num == MAX_VECTORS3D ){
        yyerror( "too many vector3d\n");
        ERROR_RETURN;                                
    } else {
        $$.v[ $$.v_num ] = $3;
        $$.v_num ++;
    }
}
| vector3d { $$.v[0] = $1; $$.v_num = 1; }
 ;


vector3d: FLOAT ';' FLOAT ';' FLOAT ';' {
    $$.x = $1;
    $$.y = $3;
    $$.z = $5;
}
 ;


meshtexturecoords: MESHTEXTURECOORDS '{' INTEGER ';' vectors2d ';' '}' {
    extern mesh *obj_cur_mesh;
    obj_cur_mesh->mc.coords = $5;
    
    if( $5.v_num != $3 ){
        yyerror( "bad coords num\n");
        ERROR_RETURN;                                
    }
}
 ;


vectors2d: vectors2d ',' vector2d {
    if( $$.v_num == MAX_VECTORS2D ){
        yyerror( "too many vector2d\n");
        ERROR_RETURN;                                
    } else {
        $$.v[$$.v_num] = $3;
        $$.v_num++;
    }
}
| vector2d { $$.v[0] = $1; $$.v_num = 1; }
 ;


vector2d: FLOAT ';' FLOAT ';' { $$.x = $1; $$.y = $3; }
 ;



%%


void yyerror( char *m )
{
    extern int obj_num_lines;
    fprintf(stderr, "YYERROR in %d: %s\n", obj_num_lines, m);
}


void obj_meshes_dump( meshes *m )
{
    int j,i;
    fprintf(stderr,"Dumping meshes. m_num=%d\n", m->m_num );
    for(i=0;i<m->m_num;i++){
        for(j=0;j<m->mp[i]->v.v_num;j++){
            fprintf(stderr,"M[%d]V[%d]=%f,%f,%f\n", i,j,
                    m->mp[i]->v.v[j].x,
                    m->mp[i]->v.v[j].y,
                    m->mp[i]->v.v[j].z );
        }
        for(j=0;j<m->mp[i]->f.f_num;j++){
            fprintf(stderr,"M[%d]F[%d]=%d,%d,%d,%d\n", i,j,
                    m->mp[i]->f.f[j].vinds[0],
                    m->mp[i]->f.f[j].vinds[1],
                    m->mp[i]->f.f[j].vinds[2],
                    m->mp[i]->f.f[j].vinds[3]
                    );
        }
        for(j=0;j<m->mp[i]->ml.matlist.mat_num;j++){
            fprintf(stderr,"M[%d]ML.MAT[%d]=(%f,%f,%f,%f),%f,(%f,%f,%f),(%f,%f,%f),%s\n",
                    i,j,
                    m->mp[i]->ml.matlist.mat[j].facecol.r,
                    m->mp[i]->ml.matlist.mat[j].facecol.g,
                    m->mp[i]->ml.matlist.mat[j].facecol.b,
                    m->mp[i]->ml.matlist.mat[j].facecol.a,                    
                    m->mp[i]->ml.matlist.mat[j].power,
                    m->mp[i]->ml.matlist.mat[j].specular.r,
                    m->mp[i]->ml.matlist.mat[j].specular.g,
                    m->mp[i]->ml.matlist.mat[j].specular.b,
                    m->mp[i]->ml.matlist.mat[j].emissive.r,
                    m->mp[i]->ml.matlist.mat[j].emissive.g,
                    m->mp[i]->ml.matlist.mat[j].emissive.b,
                    m->mp[i]->ml.matlist.mat[j].texf.filename );
        }
        for(j=0;j<m->mp[i]->ml.finds.inds_num;j++){
            fprintf(stderr,"M[%d]ML.FINDS[%d]=%d\n",
                    i,j,
                    m->mp[i]->ml.finds.inds[j] );
        }
        for(j=0;j<m->mp[i]->mc.coords.v_num;j++){
            fprintf(stderr,"M[%d]MC.COORDS[%d]=(%f,%f)\n",
                    i,j,
                    m->mp[i]->mc.coords.v[j].x,
                    m->mp[i]->mc.coords.v[j].y );
        }
        for(j=0;j<m->mp[i]->mn.v.v_num;j++){
            fprintf(stderr,"M[%d]MN.V[%d]=(%f,%f,%f)\n",
                    i,j,
                    m->mp[i]->mn.v.v[j].x,
                    m->mp[i]->mn.v.v[j].y,
                    m->mp[i]->mn.v.v[j].z );
                    
        }
        for(j=0;j<m->mp[i]->mn.f.f_num;j++){
            fprintf(stderr,"M[%d]MN.F[%d]=%d(%d,%d,%d,%d)\n",
                    i,j,
                    m->mp[i]->mn.f.f[j].vinds_num,
                    m->mp[i]->mn.f.f[j].vinds[0],
                    m->mp[i]->mn.f.f[j].vinds[1],
                    m->mp[i]->mn.f.f[j].vinds[2],
                    m->mp[i]->mn.f.f[j].vinds[3] );
        }
    }
}


int obj_parse_meshes_in_buf( char *buf, int sz, meshes *m )
{
    int ret;
    
    /* use global vars */
    obj_buf = buf;
    obj_sz = sz;
    obj_so_far = 0;
    obj_meshes = m;
    ret = yyparse();

    if( m->m_num == 0 ){
        return -1;
    } else {
        return 0;
    }
    return 0;
}

