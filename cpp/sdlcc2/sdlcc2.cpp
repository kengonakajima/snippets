// sdlcc2.cpp : コンソール アプリケーション用のエントリ ポイントの定義
//

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "SDL.h"
#include "objtypes.h"

#define S_WIDTH 640
#define S_HEIGHT 480

typedef struct _Object
{
	int x,y;
	int dx,dy;
} Object;

#define MAX_OBJ 10000
Object obj[MAX_OBJ];
int cur_obj=0;



SDL_Surface *screen, *image;
Uint32 TimeLeft(void);

void draw_all( void );
void new_obj(void );
void move_all(void);
int load_all_objects( void );

GLuint texture[1];

void LoadGLTextures(void);
// Lighting Info
GLfloat			gLightAmbient[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat			gLightDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat			gLightPosition[] = { 0.5, 1.0, 2.0, 1.0 };

void InitGL(int Width, int Height )
{
	glViewport( 0,0,Width, Height);

    LoadGLTextures();				// Load The Texture(s)
    glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping    
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glEnable( GL_LIGHTING);
	glLoadIdentity();
	gluPerspective( 45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,200.0f);
	glMatrixMode(GL_MODELVIEW);


	// Configure a light
	glLightfv( GL_LIGHT0, GL_AMBIENT, gLightAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, gLightDiffuse );
	glLightfv( GL_LIGHT0, GL_POSITION, gLightPosition );
	glLightModelf( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
	glEnable( GL_COLOR_MATERIAL );
	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHTING );
	

}


int
main(int argc, char *argv[])
{
#if 0
    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
#endif
	SDL_Init( SDL_INIT_VIDEO );

//	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);
//	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);
	OutputDebugString( "hogehoge\n" );

//	screen = SDL_SetVideoMode( S_WIDTH, S_HEIGHT, 24, SDL_SWSURFACE | SDL_OPENGL  );
	screen = SDL_SetVideoMode( S_WIDTH, S_HEIGHT, 24, SDL_OPENGLBLIT | SDL_SWSURFACE );
	OutputDebugString( "hogehoge2\n" );
	if ( screen == NULL ) {
		OutputDebugString( "hogehoge3n");
		fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}

//	SDL_WM_SetCaption( "Ringoringo", NULL );

	OutputDebugString( "hogehoge4\n");
	InitGL(640,480);

	OutputDebugString( "hogehoge5\n");

	image = SDL_LoadBMP( "ce64.bmp" );
	if( image == NULL ) {
		fprintf( stderr, "Cannot load BMP\n" );
		exit(1);
	}
	OutputDebugString( "hogehoge6\n");

    if( load_all_objects() < 0 ){
        fprintf(stderr,"Cannot load objects\n" );
        OutputDebugString( "fucking load_objects\n");
        exit(1);
    }

	while(1){
		SDL_Event event;
		SDL_PollEvent(&event);

		switch (event.type) {
		case SDL_KEYDOWN:
			char *s;
			s = SDL_GetKeyName( event.key.keysym.sym );
			if( strcmp( s, "a" ) == 0 ){
				new_obj();
				new_obj();
				new_obj();
				new_obj();
			}
			printf("The %s key was pressed!\n",
				SDL_GetKeyName(event.key.keysym.sym));
			
	        break;
		case SDL_QUIT:
			exit(0);
		}

		move_all();

		draw_all();

		SDL_Delay(TimeLeft());
 
	}

	return 0;
}
void new_obj(void )
{
	if( cur_obj == MAX_OBJ ) return;
	obj[cur_obj].x = rand() % 300;
	obj[cur_obj].y = rand() % 300;
	obj[cur_obj].dx = 1 + rand() % 3;
	obj[cur_obj].dy = 1 + rand() % 3;
	cur_obj++;
	
}
void move_all(void)
{
	int i;
	for(i=0;i<cur_obj;i++){
		int nx = obj[i].x + obj[i].dx;
		int ny = obj[i].y + obj[i].dy;

		if( nx < 0 || nx >= (S_WIDTH-image->w) ) {
			obj[i].dx *= -1;
			continue;
		}
		if( ny < 0 || ny >=  (S_HEIGHT-image->h) ){
			obj[i].dy *= -1;
			continue;
		}
		obj[i].x = nx;
		obj[i].y = ny;
	}
}

float rtri = 0.0f;


void
do_pyramid(float x, float y, float z, float r )
{
	glLoadIdentity();
	glTranslatef(x,y,z);
	glRotatef( r, r, 1.0f, r);


#if 0    
	glBegin(GL_POLYGON);
	glColor3f(1.0f,0.0f,0.0f);			// Set The Color To Red
	glVertex3f(0.0f, 1.0f, 0.0f);		        // Top of triangle (front)
	glColor3f(0.0f,1.0f,0.0f);			// Set The Color To Green
	glVertex3f(-1.0f,-1.0f, 1.0f);		// left of triangle (front)
	glColor3f(0.0f,0.0f,1.0f);			// Set The Color To Blue
	glVertex3f(1.0f,-1.0f, 1.0f);		        // right of traingle (front)	

  // right face of pyramid
	glColor3f(1.0f,0.0f,0.0f);			// Red
	glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Right)
	glColor3f(0.0f,0.0f,1.0f);			// Blue
	glVertex3f( 1.0f,-1.0f, 1.0f);		// Left Of Triangle (Right)
	glColor3f(0.0f,1.0f,0.0f);			// Green
	glVertex3f( 1.0f,-1.0f, -1.0f);		// Right Of Triangle (Right)

  // back face of pyramid
	glColor3f(1.0f,0.0f,0.0f);			// Red
	glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Back)
	glColor3f(0.0f,1.0f,0.0f);			// Green
	glVertex3f( 1.0f,-1.0f, -1.0f);		// Left Of Triangle (Back)
	glColor3f(0.0f,0.0f,1.0f);			// Blue
	glVertex3f(-1.0f,-1.0f, -1.0f);		// Right Of Triangle (Back)

  // left face of pyramid.
	glColor3f(1.0f,0.0f,0.0f);			// Red
	glVertex3f( 0.0f, 1.0f, 0.0f);		// Top Of Triangle (Left)
	glColor3f(0.0f,0.0f,1.0f);			// Blue
	glVertex3f(-1.0f,-1.0f,-1.0f);		// Left Of Triangle (Left)
	glColor3f(0.0f,1.0f,0.0f);			// Green
	glVertex3f(-1.0f,-1.0f, 1.0f);		// Right Of Triangle (Left)
	glEnd();
#endif

    glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.    
    glBegin(GL_QUADS);		                // begin drawing a cube
    // Front Face (note that the texture's corners have to match the quad's corners)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    
    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
	
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    
    // Bottom Face       
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    
    glEnd();                                    // done with the polygon.
    
}


/*
  全オブジェクトをロードする
 */


/*  オブジェクトファイルを1個読みこむ */
struct object *
load_one_object_file( char *fn )
{
    /* ファイルの中身 */
    char buf[1024 * 128];
    FILE *fp;
    int sz;
    struct object *obj;
    
    fp = fopen( fn, "rb");
    if( !fp ){
        OutputDebugString( "cannot load file\n" );
        return NULL;
    }
    sz = fread( buf, 1, sizeof(buf), fp );
    fclose(fp);

    obj = (struct object*) malloc( sizeof( struct object ));

#if 0
    if( parse_object_in_buf( buf,sz ) < 0 ){
        free(obj);
        return NULL;
    }
#endif
    return obj;
}

struct object *obj1;

int
load_all_objects( void )
{
    
    if( ( obj1 = load_one_object_file( "test.x" ) ) == 0 ){
        return -1;
    }
}

/*
  3Dのオブジェクトをすべて描画する。
 */
GLfloat lightZeroPosition[] = {10.0, 14.0, 10.0, 1.0};
GLfloat lightZeroColor[] = {0.8, 1.0, 0.8, 1.0}; /* green-tinted */
void
draw_3d_objects_all( void )
{
    float x = 6.0;
    float y = 6.0;
    float z = -38.0;
    static float r = 2.1;
    float k = (float) atof( "8.1f");

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    r+= 2.0;
    
	glLoadIdentity();
	glTranslatef(x,y,z);
	glRotatef( r, r, 1.0f, r);


    glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.    
    glBegin(GL_QUADS);		                // begin drawing a cube
    // Front Face (note that the texture's corners have to match the quad's corners)
    glTexCoord2f(0.0f, 0.0f); 
	glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); 
	glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); 
	glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); 
	glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    
    // Back Face
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
	
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    
    // Bottom Face       
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);	// Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);	// Bottom Left Of The Texture and Quad
    
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	// Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);	// Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);	// Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);	// Top Left Of The Texture and Quad
    
    glEnd();                                    // done with the polygon.

    
}


void draw_all( void )
{
	int i;



#if 1	
	{
		SDL_Rect d;
		Uint32 c = 1111;
		d.x = 0;
		d.y = 0;
		d.w = S_WIDTH;
		d.h = S_HEIGHT;

		SDL_FillRect( screen, &d, c);
	}
#endif

	for(i=0;i<cur_obj;i++){
		SDL_Rect dest;
	    dest.x = obj[i].x;
	    dest.y = obj[i].y;
	    dest.w = image->w;
	    dest.h = image->h;
	    SDL_BlitSurface(image, NULL, screen, &dest);

	}

    /* 画面の変更された部分を更新する */


    draw_3d_objects_all();
    
#if 0
	rtri += 2.0f;

	{
		int i,j,k;
		int pyn=3;
		for(i=-pyn;i<=pyn;i++){
			for(j=-pyn;j<=pyn;j++){
				for(k=-pyn;k<=pyn;k++){
					do_pyramid( (float)(j*3) , (float)(i*3), -40.0f - (k*3) , rtri);
				}
			}
		}
	}
#endif
    


	{
		SDL_Rect d;
		d.x = 0;
		d.y = 0;
		d.w = S_WIDTH;
		d.h = S_HEIGHT;
		SDL_UpdateRects(screen, 1, &d);
			
	}

	SDL_GL_SwapBuffers();    
    
}


SDL_Surface *LoadBMP(char *filename)
{
    Uint8 *rowhi, *rowlo;
    Uint8 *tmpbuf, tmpch;
    SDL_Surface *image;
    int i, j;

    image = SDL_LoadBMP(filename);
    if ( image == NULL ) {
        fprintf(stderr, "Unable to load %s: %s\n", filename, SDL_GetError());
        return(NULL);
    }
#if 1
    /* GL surfaces are upsidedown and RGB, not BGR :-) */
    tmpbuf = (Uint8 *)malloc(image->pitch);
    if ( tmpbuf == NULL ) {
        fprintf(stderr, "Out of memory\n");
        return(NULL);
    }
    rowhi = (Uint8 *)image->pixels;
    rowlo = rowhi + (image->h * image->pitch) - image->pitch;
    for ( i=0; i<image->h/2; ++i ) {
        for ( j=0; j<image->w; ++j ) {
            tmpch = rowhi[j*3];
            rowhi[j*3] = rowhi[j*3+2];
            rowhi[j*3+2] = tmpch;
            tmpch = rowlo[j*3];
            rowlo[j*3] = rowlo[j*3+2];
            rowlo[j*3+2] = tmpch;
        }
        memcpy(tmpbuf, rowhi, image->pitch);
        memcpy(rowhi, rowlo, image->pitch);
        memcpy(rowlo, tmpbuf, image->pitch);
        rowhi += image->pitch;
        rowlo -= image->pitch;
    }
    free(tmpbuf);
#endif    
    return(image);
}

// Load Bitmaps And Convert To Textures
void LoadGLTextures(void)
{	
    // Load Texture
    SDL_Surface *image1;
    
    image1 = LoadBMP("ce64_24.bmp");
    if (!image1) {
        SDL_Quit();
        exit(1);
    }

    OutputDebugString( "hogehoge099\n");
    // Create Texture	
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture
    OutputDebugString( "hogehoge0999\n");
    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->w, image1->h, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->pixels);
    OutputDebugString( "hogehoge09997\n"    );
}



#define TICK_INTERVAL    16

Uint32 TimeLeft(void)
{
    static Uint32 next_time = 0;
    Uint32 now;

    now = SDL_GetTicks();
    if ( next_time <= now ) {
        next_time = now+TICK_INTERVAL;
        return(0);
    }
    return(next_time-now);
}

