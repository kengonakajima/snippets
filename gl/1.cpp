#include <math.h>
//#include <windows.h>
//#include <GL/gl.h>
#include <GLUT/glut.h>

#define ANGLE 0.2

GLfloat translate[] = {
	1 , 0 , 0 , 0 ,
	0 , 1 , 0 , 0 ,
	0 , 0 , 1 , 0 ,
	0 , 0 , 0 , 1
};

void disp( void ) {
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_POLYGON);
    glColor3f(1 , 0 , 0);
    glVertex2f(-0.9 , -0.9);
    glColor3f(0 , 1 , 0);
    glVertex2f(0 , 0.9);
    glColor3f(0 , 0 , 1);
    glVertex2f(0.9 , -0.9);
	glEnd();

	glFlush();
}

void timer(int value) {
	glMultMatrixf(translate);
	glutPostRedisplay();
	glutTimerFunc(50 , timer , 0);
}

int main(int argc , char ** argv) {
	glutInit(&argc , argv);
	glutInitWindowPosition(100 , 50);
	glutInitWindowSize(400 , 300);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

	glutCreateWindow("Kitty on your lap");
	glutDisplayFunc(disp);
	glutTimerFunc(100 , timer , 0);

	translate[0] = cos(ANGLE);
	translate[2] = -sin(ANGLE);
	translate[8] = sin(ANGLE);
	translate[10] = cos(ANGLE);
	glutMainLoop();
	return 0;
}
