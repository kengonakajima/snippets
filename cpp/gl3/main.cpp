#include <stdio.h>
#include <unistd.h>

#include "mrs.hpp"

#include <OpenGL/gl3.h>
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include "GLFW/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"

extern "C" {

// brew provides this
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include "libavutil/imgutils.h"

};

#include "cumino.h"

const char vertex_uv_color_glsl[] =
    "varying vec2 vUv;\n"
    "varying vec4 vColor;\n"
    "attribute vec4 color;\n"
    "attribute vec2 uv;\n"
    "attribute vec3 position;\n"
    "uniform mat4 modelViewMatrix;\n"
    "uniform mat4 projectionMatrix;\n"
    "void main()\n"
    "{\n"
    "  vUv = uv;\n"
    "  vColor = color;\n"
    "  vec4 mvPosition = modelViewMatrix * vec4(position, 1.0);\n"
    "  gl_Position = projectionMatrix * mvPosition;\n"
    "}\n";
const char fragment_uv_color_glsl[] =
    "uniform sampler2D texture;\n"
    "uniform vec4 meshcolor;\n"
    "varying vec2 vUv;\n"
    "varying vec4 vColor;\n"
    "void main()\n"
    "{\n"
    "  vec4 tc = texture2D(texture,vUv);\n"
    "  if(tc.a<0.01) discard; else gl_FragColor = vec4( tc.r * meshcolor.r * vColor.r * tc.a * vColor.a * meshcolor.a, tc.g * meshcolor.g * vColor.g * tc.a * vColor.a * meshcolor.a, tc.b * meshcolor.b * vColor.b * tc.a * vColor.a * meshcolor.a, tc.a * meshcolor.a * vColor.a);\n"
    "}\n";

const char vsh_simple_glsl[] =
    //    "# version 330 core\n"
    "#version 330 core\n"
    "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
    // Notice that the "1" here equals the "1" in glVertexAttribPointer
    "layout(location = 1) in vec3 vertexColor;\n"
    "layout(location = 2) in vec2 vertexUV;\n"    
    "uniform mat4 MVP;\n"
    "out vec3 fragmentColor;\n"
    "out vec2 UV;\n"            
    "void main() {\n"
    //    " gl_Position.xyz=vertexPosition_modelspace;\n"
    //    " gl_Position.w=1.0;\n"
    " gl_Position=MVP*vec4(vertexPosition_modelspace,1);\n"
    "  fragmentColor = vertexColor;\n"
    "  UV = vertexUV;\n"            
    "}\n";

const char fsh_red_glsl[] = 
    "# version 330 core\n"
    "in vec3 fragmentColor;\n"
    "in vec2 UV;\n"
    "out vec3 color;\n"
    "uniform sampler2D myTextureSampler;\n"
    "void main(){\n"
    //    "  color = vec3(1,0,0);\n"
    "  vec3 texcol = texture(myTextureSampler,UV).rgb;\n"        
    "  color = vec3( texcol.r * fragmentColor.r, texcol.g * fragmentColor.g, texcol.b * fragmentColor.b);\n"
    "}\n";

const char fsh_yuv_glsl[] = 
    "# version 330 core\n"
    "in vec3 fragmentColor;\n"
    "in vec2 UV;\n"
    "out vec3 color;\n"
    "uniform sampler2D myTextureSampler;\n"
    "void main(){\n"
    //    "  color = vec3(1,0,0);\n"
    "  vec3 texcol = texture(myTextureSampler,UV).rgb;\n"        
    "  vec3 rgb = vec3( texcol.r * fragmentColor.r, texcol.g * fragmentColor.g, texcol.b * fragmentColor.b);\n"
    "  color.x = rgb.r * 0.299 + rgb.g * 0.587 + rgb.b * 0.114;\n"
    "  color.y = rgb.r * -0.169 + rgb.g * -0.331 + rgb.b * 0.5 + 0.5;\n"
    "  color.z = rgb.r * 0.5 + rgb.g * -0.419 + rgb.b * -0.081 + 0.5;\n"
    "}\n";


const int SCRW=1280/2, SCRH=720/2;

////////

void errorCallback(int code, const char* err_str) {
    print("glfw error: code:%d str:%s",code,err_str);
}
void keyboardCallback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
    print("kbd: key:%d action:%d",key,action);
    if(key==81) {
        glfwTerminate();
        exit(0); //q on mac   
    }
}

GLuint LoadShaders(const char * v_glsl,const char * f_glsl){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	glShaderSource(VertexShaderID, 1, &v_glsl , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		char emsg[1000];
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, emsg);
		print("vsh comp error: %s", emsg);
	}

	// Compile Fragment Shader
	glShaderSource(FragmentShaderID, 1, &f_glsl , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
        char emsg[1000];
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, emsg);
		print("fsh comp error: %s", emsg);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
        char emsg[1000];
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, emsg);
		print("link error: %s", emsg);
	}
	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

GLuint loadBMP(const char *fname) {

    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;

    FILE *fp=fopen(fname,"rb");
    if(!fp) {
        print("cant open file");
        exit(1);
    }
    if(fread(header,1,54,fp)!=54) {
        print("header erorr");
        exit(1);
    }
    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
        return 0;
    }

    // Read ints from the byte array
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way

    // Create a buffer
    data = new unsigned char [imageSize];

    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,fp);

    //Everything is in memory now, the file can be closed
    fclose(fp);


    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
    
}

// out: yuv420p (planer. not sp:semiplaner)
// rgb: w*h*3 out: w*h*3/2
void RGBtoYUV420Planar(unsigned char *rgb, int width, int height, unsigned char *out) {
    int frameSize = width * height;
    int yIndex = 0;
    int uIndex = frameSize;
    int vIndex = frameSize + (frameSize / 4);
    int r, g, b, y, u, v;
    int index = 0;  

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            r = rgb[index * 3] & 0xff;
            g = rgb[index * 3 + 1] & 0xff;
            b = rgb[index * 3 + 2] & 0xff;

            y = (int)(0.257 * r + 0.504 * g + 0.098 * b) + 16;
            u = (int)(0.439 * r - 0.368 * g - 0.071 * b) + 128;
            v = (int)(-0.148 * r - 0.291 * g + 0.439 * b) + 128;

            out[yIndex++] = (unsigned char)((y < 0) ? 0 : ((y > 255) ? 255 : y));

            if (j % 2 == 0 && index % 2 == 0) { 
                out[uIndex++] = (unsigned char)((u < 0) ? 0 : ((u > 255) ? 255 : u));
                out[vIndex++] = (unsigned char)((v < 0) ? 0 : ((v > 255) ? 255 : v));
            }

            index++;
        }
    }
}


// mbp2018
// 2.8% for no-cap
// 13% for 640x480
// 27% for 1280x720
// 5.7% for 1280x720, skip row1,pix1

//AVCodecID g_codec_id=AV_CODEC_ID_H264; 
AVCodecID g_codec_id=AV_CODEC_ID_MPEG1VIDEO;
void capture() {


    static AVCodec *codec=NULL;
    static AVCodecContext *c=NULL;
    static AVFrame *frame=NULL;
    static FILE *fp=0;    
    if(!codec) {
        print("init codec");
        codec=avcodec_find_encoder(g_codec_id);
        if(!codec) {
            print("codec not found");
            exit(1);
        }
        c=avcodec_alloc_context3(codec);
        print("alloc codec:%p",c);

        c->bit_rate=400000;
        c->width=SCRW;
        c->height=SCRH;
        c->time_base=(AVRational){1,60};
        c->gop_size=10; // emit one intra frame every 10 frames
        c->max_b_frames=1;
        c->pix_fmt=AV_PIX_FMT_YUV420P;

        if(avcodec_open2(c,codec,0)<0) {
            print("cant open codec");
            exit(1);
        }
        char path[1000];
        if(g_codec_id==AV_CODEC_ID_H264) {
            sprintf(path,"out264.mp4");
        } else {
            sprintf(path,"outmpeg1.mpeg");
        }
        fp=fopen(path,"wb");
        if(!fp) {
            print("cant open outfile");
            exit(1);
        }
        if(g_codec_id==AV_CODEC_ID_H264) {
            unsigned char starter[4]={0,0,0,1};
            fwrite(starter, 4,1,fp);
        }
        frame=av_frame_alloc();
        if(!frame) {
            print("frame cant alloc");
            exit(1);
        }
        frame->format = c->pix_fmt;
        frame->width=c->width;
        frame->height=c->height;

        int ret=av_image_alloc(frame->data, frame->linesize, c->width, c->height, c->pix_fmt, 1 );
        if(ret<0){
            print("av_image_alloc error:%d",ret);
            exit(1);
        }
        print("linesize:%d,%d,%d",frame->linesize[0], frame->linesize[1], frame->linesize[2]);
        
    }

    
    const int RETINA=2;
	static bool init = false;
	static GLuint pbo_id;
	size_t pbo_size = SCRW * SCRH * 3 * RETINA * RETINA;
	if (!init) {
		init = true;
		print("initialize pbo");
		glGenBuffers(1, &pbo_id);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
		glBufferData(GL_PIXEL_PACK_BUFFER, pbo_size, 0, GL_DYNAMIC_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	}

    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
	glReadPixels(0, 0, SCRW*RETINA, SCRH*RETINA, GL_RGB, GL_UNSIGNED_BYTE, 0);
	GLubyte *ptr = (GLubyte*)glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, pbo_size, GL_MAP_READ_BIT);

    //    static unsigned char *g_pixels;
    //	if (!g_pixels)g_pixels = (unsigned char*) malloc(SCRW*SCRH * 3*RETINA*RETINA); // RGB * retina*retina

    // ptr: yuvyuvyuv....
    // data[0]: yyyyy data[1]:uuuu data[2]:vvvvv 
	if (ptr) {
        // memcpy(g_pixels, ptr, pbo_size); else print("updategenvid ptr null error:%d",glGetError());
        /* Y */
#if 1       
        for(int y=0;y<c->height;y++) {
            for(int x=0;x<c->width;x++) {
                int at=x*3*RETINA + (SCRH-1-y)*3*RETINA*SCRW*2;
                frame->data[0][y * frame->linesize[0] + x] = ptr[at];
            }
        }
        /* Cb and Cr */
        for(int y=0;y<c->height/2;y++) {
            for(int x=0;x<c->width/2;x++) {
                int at=(x*2)*3*RETINA + ((SCRH/2-1-y)*2)*3*RETINA*SCRW*2;                
                frame->data[1][y * frame->linesize[1] + x] = ptr[at+1];
                frame->data[2][y * frame->linesize[2] + x] = ptr[at+2];
            }
        }
#endif        
        static int framecnt=0;
        frame->pts=framecnt;
#if 1
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data=NULL;
        pkt.size=0;
        int got_output=0;
        int ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
        if(ret<0) {
            print("encode_video2 fail:%d",ret);
            exit(1);
        }
        if(got_output) {
            //            print("write frame %d  size:%d",framecnt, pkt.size);
            fwrite(pkt.data, 1, pkt.size, fp);
            av_free_packet(&pkt);
        }
#endif
        framecnt++;
    }

	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    // dump, 20pixごとに
#if 0
    int u=20;
    for(int y=0;y<SCRH*RETINA;y+=u) {
        for(int x=0;x<SCRW*RETINA;x+=u) {
            unsigned int at = x*3 + y*SCRW*3*RETINA;
            unsigned char r=g_pixels[at];
            unsigned char g=g_pixels[at+1];
            unsigned char b=g_pixels[at+2];
            if(r>0) prt("*"); else prt(".");
            //            if(x==640 && y==360) prt(" %d,%d,%d", r,g,b);
        }
        print("");
    }
    print("--");

    //    FILE *fp=fopen("dumped","wb");
    //    fwrite(g_pixels, 1, SCRW*SCRH*3, fp);
    //    fclose(fp);
    
#endif

#if 0
    int i,got_output;
    for(i=0;i<25;i++) {
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data=NULL;
        pkt.size=0;
        
        /* prepare a dummy image */
        /* Y */
        for(int y=0;y<c->height;y++) {
            for(int x=0;x<c->width;x++) {
                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
            }
        }
        /* Cb and Cr */
        for(int y=0;y<c->height/2;y++) {
            for(int x=0;x<c->width/2;x++) {
                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
            }
        }
        frame->pts=i;

        int ret = avcodec_encode_video2(c, &pkt, frame, &got_output);
        if(ret<0) {
            print("encode_video2 fail:%d",ret);
            exit(1);
        }
        if(got_output) {
            print("write frame %d  size:%d",i, pkt.size);
            fwrite(pkt.data, 1, pkt.size, fp);
            av_free_packet(&pkt);
        }
    }
    // delayed frames
    for (got_output = 1; got_output; i++) {
        AVPacket pkt;
        av_init_packet(&pkt);
        pkt.data=NULL;
        pkt.size=0;
        int ret = avcodec_encode_video2(c, &pkt, NULL, &got_output);
        if (ret < 0) {
            print( "Error encoding frame\n");
            exit(1);
        }
 
        if (got_output) {
            print("Write frame %3d (size=%5d)\n", i, pkt.size);
            fwrite(pkt.data, 1, pkt.size, fp);
            av_free_packet(&pkt);
        }
    }
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    
    fwrite(endcode, 1, sizeof(endcode), fp);
    fclose(fp);


    avcodec_close(c);
    av_free(c);
    av_freep(&frame->data[0]);
    av_frame_free(&frame);    
#endif 



}    




int main() {
    GLFWwindow *window;

    if( !glfwInit() ) {
        print("can't init glfw");
        exit(1);
    }
    
    glfwSetErrorCallback(errorCallback);

    //     glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER,1); not available in glfw 3.2
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow( SCRW,SCRH, "test", NULL, NULL );
    if(!window) {
        print("createwindow error:");
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback( window, keyboardCallback );
    
    //    glfwSetWindowCloseCallback()
    glfwSwapInterval(1);
    glClearColor(0,0,0,1);


    GLuint program_id = LoadShaders( vsh_simple_glsl, fsh_red_glsl ); 
    GLuint program_id_yuv = LoadShaders( vsh_simple_glsl, fsh_yuv_glsl );
    
    //////////
    GLuint vert_ary_id;
    glGenVertexArrays(1,&vert_ary_id);
    glBindVertexArray(vert_ary_id);

#if 0    
    static const GLfloat g_vertex_buffer_data[] = {
                                                   -1.0f, -1.0f, 0.0f,
                                                   1.0f, -1.0f, 0.0f,
                                                   0.0f,  1.0f, 0.0f,
    };
#endif
    
    static const GLfloat g_vertex_buffer_data[] = {
                                                   -1.0f,-1.0f,-1.0f, // triangle 1 : begin
                                                   -1.0f,-1.0f, 1.0f,
                                                   -1.0f, 1.0f, 1.0f, // triangle 1 : end
                                                   1.0f, 1.0f,-1.0f, // triangle 2 : begin
                                                   -1.0f,-1.0f,-1.0f,
                                                   -1.0f, 1.0f,-1.0f, // triangle 2 : end
                                                   1.0f,-1.0f, 1.0f,
                                                   -1.0f,-1.0f,-1.0f,
                                                   1.0f,-1.0f,-1.0f,
                                                   1.0f, 1.0f,-1.0f,
                                                   1.0f,-1.0f,-1.0f,
                                                   -1.0f,-1.0f,-1.0f,
                                                   -1.0f,-1.0f,-1.0f,
                                                   -1.0f, 1.0f, 1.0f,
                                                   -1.0f, 1.0f,-1.0f,
                                                   1.0f,-1.0f, 1.0f,
                                                   -1.0f,-1.0f, 1.0f,
                                                   -1.0f,-1.0f,-1.0f,
                                                   -1.0f, 1.0f, 1.0f,
                                                   -1.0f,-1.0f, 1.0f,
                                                   1.0f,-1.0f, 1.0f,
                                                   1.0f, 1.0f, 1.0f,
                                                   1.0f,-1.0f,-1.0f,
                                                   1.0f, 1.0f,-1.0f,
                                                   1.0f,-1.0f,-1.0f,
                                                   1.0f, 1.0f, 1.0f,
                                                   1.0f,-1.0f, 1.0f,
                                                   1.0f, 1.0f, 1.0f,
                                                   1.0f, 1.0f,-1.0f,
                                                   -1.0f, 1.0f,-1.0f,
                                                   1.0f, 1.0f, 1.0f,
                                                   -1.0f, 1.0f,-1.0f,
                                                   -1.0f, 1.0f, 1.0f,
                                                   1.0f, 1.0f, 1.0f,
                                                   -1.0f, 1.0f, 1.0f,
                                                   1.0f,-1.0f, 1.0f
    };
    
    static const GLfloat g_color_buffer_data[] = {
                                                  0.583f,  0.771f,  0.014f,
                                                  0.609f,  0.115f,  0.436f,
                                                  0.327f,  0.483f,  0.844f,
                                                  0.822f,  0.569f,  0.201f,
                                                  0.435f,  0.602f,  0.223f,
                                                  0.310f,  0.747f,  0.185f,
                                                  0.597f,  0.770f,  0.761f,
                                                  0.559f,  0.436f,  0.730f,
                                                  0.359f,  0.583f,  0.152f,
                                                  0.483f,  0.596f,  0.789f,
                                                  0.559f,  0.861f,  0.639f,
                                                  0.195f,  0.548f,  0.859f,
                                                  0.014f,  0.184f,  0.576f,
                                                  0.771f,  0.328f,  0.970f,
                                                  0.406f,  0.615f,  0.116f,
                                                  0.676f,  0.977f,  0.133f,
                                                  0.971f,  0.572f,  0.833f,
                                                  0.140f,  0.616f,  0.489f,
                                                  0.997f,  0.513f,  0.064f,
                                                  0.945f,  0.719f,  0.592f,
                                                  0.543f,  0.021f,  0.978f,
                                                  0.279f,  0.317f,  0.505f,
                                                  0.167f,  0.620f,  0.077f,
                                                  0.347f,  0.857f,  0.137f,
                                                  0.055f,  0.953f,  0.042f,
                                                  0.714f,  0.505f,  0.345f,
                                                  0.783f,  0.290f,  0.734f,
                                                  0.722f,  0.645f,  0.174f,
                                                  0.302f,  0.455f,  0.848f,
                                                  0.225f,  0.587f,  0.040f,
                                                  0.517f,  0.713f,  0.338f,
                                                  0.053f,  0.959f,  0.120f,
                                                  0.393f,  0.621f,  0.362f,
                                                  0.673f,  0.211f,  0.457f,
                                                  0.820f,  0.883f,  0.371f,
                                                  0.982f,  0.099f,  0.879f
    };

    static const GLfloat g_uv_buffer_data[] = {
                                               0.000059f, 1.0f-0.000004f,
                                               0.000103f, 1.0f-0.336048f,
                                               0.335973f, 1.0f-0.335903f,
                                               1.000023f, 1.0f-0.000013f,
                                               0.667979f, 1.0f-0.335851f,
                                               0.999958f, 1.0f-0.336064f,
                                               0.667979f, 1.0f-0.335851f,
                                               0.336024f, 1.0f-0.671877f,
                                               0.667969f, 1.0f-0.671889f,
                                               1.000023f, 1.0f-0.000013f,
                                               0.668104f, 1.0f-0.000013f,
                                               0.667979f, 1.0f-0.335851f,
                                               0.000059f, 1.0f-0.000004f,
                                               0.335973f, 1.0f-0.335903f,
                                               0.336098f, 1.0f-0.000071f,
                                               0.667979f, 1.0f-0.335851f,
                                               0.335973f, 1.0f-0.335903f,
                                               0.336024f, 1.0f-0.671877f,
                                               1.000004f, 1.0f-0.671847f,
                                               0.999958f, 1.0f-0.336064f,
                                               0.667979f, 1.0f-0.335851f,
                                               0.668104f, 1.0f-0.000013f,
                                               0.335973f, 1.0f-0.335903f,
                                               0.667979f, 1.0f-0.335851f,
                                               0.335973f, 1.0f-0.335903f,
                                               0.668104f, 1.0f-0.000013f,
                                               0.336098f, 1.0f-0.000071f,
                                               0.000103f, 1.0f-0.336048f,
                                               0.000004f, 1.0f-0.671870f,
                                               0.336024f, 1.0f-0.671877f,
                                               0.000103f, 1.0f-0.336048f,
                                               0.336024f, 1.0f-0.671877f,
                                               0.335973f, 1.0f-0.335903f,
                                               0.667969f, 1.0f-0.671889f,
                                               1.000004f, 1.0f-0.671847f,
                                               0.667979f, 1.0f-0.335851f
    };
    
    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER,colorbuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(g_color_buffer_data),g_color_buffer_data,GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER,uvbuffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(g_uv_buffer_data),g_uv_buffer_data,GL_STATIC_DRAW);

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCRW/(float)SCRH, 0.1f, 100.0f);
    // Or, for an ortho camera :
    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates


    // Get a handle for our "MVP" uniform
    // Only during the initialisation
    GLuint matrix_id = glGetUniformLocation(program_id, "MVP");

    // tex
    GLuint tex = loadBMP("uvtemplate.bmp");
	GLuint texture_uniform_loc  = glGetUniformLocation(program_id, "myTextureSampler");

    //////////

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);


    //
    mrs_initialize();


    //
    
    print("start");
    while(1) {
        if(glfwWindowShouldClose(window) )break;

        ////////////

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program_id_yuv);

        // Send our transformation to the currently bound shader, in the "MVP" uniform
        // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
        // Camera matrix
        static float r=0;
        r+=0.1;
        glm::mat4 view = glm::lookAt(
                                     glm::vec3(::sin(r)*4,3,3), // Camera is at (4,3,3), in World Space
                                     glm::vec3(0,0,0), // and looks at the origin
                                     glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                     );

        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 model = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 mvp = projection * view * model; // Remember, matrix multiplication is the other way around

        
        glUniformMatrix4fv(matrix_id, 1, GL_FALSE, &mvp[0][0]);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(texture_uniform_loc, 0);


        // send buffer
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(
                              1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                              3,                                // size
                              GL_FLOAT,                         // type
                              GL_FALSE,                         // normalized?
                              0,                                // stride
                              (void*)0                          // array buffer offset
                              );

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER,uvbuffer );
        glVertexAttribPointer(
                              2,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              (void*)0
                              );
        

        
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);        


        ////////////
        double st=now();
        capture();
        double et=now();
        //        print("captime:%f",et-st);
        
        glfwSwapBuffers(window);
		glfwPollEvents();

    }
    print("end");
    glfwTerminate();
    return 0;
}
