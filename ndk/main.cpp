#include <jni.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <math.h>

#define LOG_TAG "NativeRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static GLfloat angle = 0.0f;

const char *vertexShaderCode =
    "attribute vec4 vPosition;"
    "uniform mat4 uMVPMatrix;"
    "void main() {"
    "  gl_Position = uMVPMatrix * vPosition;"
    "}";

const char *fragmentShaderCode =
    "precision mediump float;"
    "uniform vec4 vColor;"
    "void main() {"
    "  gl_FragColor = vColor;"
    "}";

GLuint loadShader(GLenum type, const char *shaderCode) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderCode, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint program;
GLuint positionHandle;
GLuint colorHandle;
GLuint mvpMatrixHandle;

GLfloat vertices[] = {
    0.0f,  0.5f, 0.0f,   // 上頂点
   -0.5f, -0.5f, 0.0f,   // 左下頂点
    0.5f, -0.5f, 0.0f    // 右下頂点
};

GLfloat color[] = {0.0f, 1.0f, 0.0f, 1.0f};  // 緑色

void setupGraphics(int width, int height) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderCode);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentShaderCode);

    program = glCreateProgram();             // プログラムオブジェクトの作成
    glAttachShader(program, vertexShader);   // バーテックスシェーダーのアタッチ
    glAttachShader(program, fragmentShader); // フラグメントシェーダーのアタッチ
    glLinkProgram(program);                  // プログラムオブジェクトのリンク

    positionHandle = glGetAttribLocation(program, "vPosition");
    colorHandle = glGetUniformLocation(program, "vColor");
    mvpMatrixHandle = glGetUniformLocation(program, "uMVPMatrix");

    glViewport(0, 0, width, height);
}

void renderFrame() {
    angle += 0.01f;

    GLfloat mvpMatrix[16] = {
        cosf(angle), -sinf(angle), 0.0f, 0.0f,
        sinf(angle),  cosf(angle), 0.0f, 0.0f,
        0.0f,         0.0f,        1.0f, 0.0f,
        0.0f,         0.0f,        0.0f, 1.0f
    };

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);

    glVertexAttribPointer(positionHandle, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(positionHandle);

    glUniform4fv(colorHandle, 1, color);
    glUniformMatrix4fv(mvpMatrixHandle, 1, GL_FALSE, mvpMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

extern "C" {
JNIEXPORT void JNICALL Java_com_example_ndkgl_NativeRenderer_init(JNIEnv *env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_example_ndkgl_NativeRenderer_step(JNIEnv *env, jobject obj);
}

JNIEXPORT void JNICALL Java_com_example_ndkgl_NativeRenderer_init(JNIEnv *env, jobject obj, jint width, jint height) {
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_com_example_ndkgl_NativeRenderer_step(JNIEnv *env, jobject obj) {
    renderFrame();
}

