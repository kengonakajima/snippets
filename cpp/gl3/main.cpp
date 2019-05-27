#include <unistd.h>

#include <OpenGL/gl3.h>
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#include "GLFW/glfw3.h"

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
    "void main() {\n"
    " gl_Position.xyz=vertexPosition_modelspace;\n"
    " gl_Position.w=1.0;\n"
    "}\n";

const char fsh_red_glsl[] = 
    "# version 330 core\n"
    "out vec3 color;\n"    
    "void main(){\n"
    "  color = vec3(1,0,0);\n"
    "}\n";

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



int main() {
    GLFWwindow *window;

    if( !glfwInit() ) {
        print("can't init glfw");
        exit(1);
    }
    
    glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow( 640,480, "test", NULL, NULL );
    if(!window) {
        print("createwindow error:");
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback( window, keyboardCallback );
    
    //    glfwSetWindowCloseCallback()
    glfwSwapInterval(1);
    glClearColor(0.2,0.2,0.2,1);


    GLuint program_id = LoadShaders( vsh_simple_glsl, fsh_red_glsl ); // fragment_uv_color_glsl );
    
    //////////
    GLuint vert_ary_id;
    glGenVertexArrays(1,&vert_ary_id);
    glBindVertexArray(vert_ary_id);

    static const GLfloat g_vertex_buffer_data[] = {
                                                   -1.0f, -1.0f, 0.0f,
                                                   1.0f, -1.0f, 0.0f,
                                                   0.0f,  1.0f, 0.0f,
    };

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    


    //////////
    
    print("start");
    while(1) {
        if(glfwWindowShouldClose(window) )break;

        ////////////

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(program_id);
        
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
        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window);
		glfwPollEvents();

    }
    print("end");
    glfwTerminate();
    return 0;
}
