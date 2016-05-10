#include <GL/glut.h>

/* 再描写時に実行される関数*/
void display(void) {
    /* 画面全体を指定した色で塗りつぶす */
    glClear(GL_COLOR_BUFFER_BIT);
    /* まだ実行されていない命令をすべて実行 */
    glFlush();
}

int main(int argc, char **argv){
    glutInit(&argc, argv);

    /* ウィンドウの位置とサイズを指定 */
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);

    /* ウィンドウを生成 */
    glutCreateWindow("test");

    /* 背景色を指定: 白 */
    glutInitDisplayMode(GLUT_RGBA);
    glClearColor(1.0, 1.0, 1.0, 1.0);

    /* 画面を再描写するときに実行される関数を指定
       (初期化、ウィンドウサイズ変更時など) */
    glutDisplayFunc(display);

    /* ウィンドウが閉じられるまでループ */
    glutMainLoop();
    return 0;
}

