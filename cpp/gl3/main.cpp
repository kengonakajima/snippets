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
    "uniform highp vec4 meshcolor;\n"
    "varying highp vec2 vUv;\n"
    "varying highp vec4 vColor;\n"
    "void main()\n"
    "{\n"
    "  highp vec4 tc = texture2D(texture,vUv);\n"
    "  if(tc.a<0.01) discard; else gl_FragColor = vec4( tc.r * meshcolor.r * vColor.r * tc.a * vColor.a * meshcolor.a, tc.g * meshcolor.g * vColor.g * tc.a * vColor.a * meshcolor.a, tc.b * meshcolor.b * vColor.b * tc.a * vColor.a * meshcolor.a, tc.a * meshcolor.a * vColor.a);\n"
    "}\n";

void errorCallback(int code, const char* err_str) {
    print("glfw error: code:%d str:%s",code,err_str);
}
void keyboardCallback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
    print("kbd: key:%d action:%d",key,action);
    if(key==81)exit(0); //q on mac
}


int main() {
    GLFWwindow *window;

    if( !glfwInit() ) {
        print("can't init glfw");
        exit(1);
    }
    
    glfwSetErrorCallback(errorCallback);
    
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

    print("start");
    while(1) {
        glfwPollEvents();
        if(glfwWindowShouldClose(window) )break;
    }
    print("end");
    glfwTerminate();
    return 0;
}
