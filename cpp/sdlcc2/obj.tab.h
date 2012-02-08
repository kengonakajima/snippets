typedef union {
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
} YYSTYPE;
#define	NAME	257
#define	FLOAT	258
#define	INTEGER	259
#define	MESH	260
#define	MATERIAL	261
#define	TEXTUREFILENAME	262
#define	MESHNORMALS	263
#define	MESHTEXTURECOORDS	264
#define	MESHMATERIALLIST	265


extern YYSTYPE yylval;
