typedef struct _vector3d {
    float x,y,z;
} vector3d;
typedef struct _vector2d {
    float x,y;
} vector2d;
typedef struct _vectors3d {
    int v_num;
#define MAX_VECTORS3D 1024
    vector3d v[MAX_VECTORS3D];
} vectors3d;
typedef struct _vectors2d {
    int v_num;
#define MAX_VECTORS2D 1024    
    vector2d v[MAX_VECTORS2D];
} vectors2d;
typedef struct _rgbacolor {
    float r,g,b,a;
} rgbacolor;
typedef struct _rgbcolor {
    float r,g,b;
} rgbcolor;
typedef struct _faceinds {
    int inds_num;
#define MAX_FACEINDS 1024    
    int inds[MAX_FACEINDS];
} faceinds;
typedef struct _face {
    int vinds_num;
    int vinds[4];
} face;
typedef struct _faces {
    int f_num;
#define MAX_FACES 1024
    face f[MAX_FACES];
} faces;

typedef struct _vertex {
    float x,y,z;
} vertex;
typedef struct _vertices {
    int v_num;
#define MAX_VERTICES 1024    
    vertex v[MAX_VERTICES];
} vertices;
typedef struct _texturefile
{
#define MAX_TEXTURE_FILE_NAME 256
    char filename[MAX_TEXTURE_FILE_NAME];
} texturefile;
typedef struct _material
{
    rgbacolor facecol;
    float power;
    rgbcolor specular;
    rgbcolor emissive;
    texturefile texf;
} material;
typedef struct _materials
{
    int mat_num;
#define MAX_MATERIAL 64        
    material mat[MAX_MATERIAL];
} materials;
typedef struct _meshmateriallist
{
    materials matlist;
    faceinds finds;
} meshmateriallist;
typedef struct _meshtexturecoords
{
    vectors2d coords;
} meshtexturecoords;
typedef struct _meshnormals
{
    vectors3d v;
    faces f;
} meshnormals;
typedef struct _mesh
{
#define MESH_NAME_MAX 256    
    char name[MESH_NAME_MAX];
    vertices v;
    faces f;
    meshmateriallist ml;
    meshtexturecoords mc;
    meshnormals mn;
} mesh;
typedef struct _meshes
{
#define MAX_MESHES 100
    int m_num;
    mesh *mp[MAX_MESHES];
} meshes;



void obj_meshes_dump( meshes *m );
int obj_parse_meshes_in_buf( char *buf, int sz, meshes *m );

extern meshes *obj_meshes;
extern mesh *obj_cur_mesh;
extern char *obj_buf;
extern int obj_sz, obj_so_far;
extern int obj_num_lines;

