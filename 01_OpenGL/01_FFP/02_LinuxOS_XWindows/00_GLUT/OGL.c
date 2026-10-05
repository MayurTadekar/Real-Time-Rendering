/**
 * S.G.M.P
 * @file: OGL.c
 * @brief: First OpenGL FreeGLUT Code (RTR Prastavna)
 * @author: Mayur S. Tadekar(mrtadekar@gmail.com)
 * @Copyright ©2020-2023 MarshallingVoid
 * @date: 13 October 2024 (Sunday) 10:26:47 PM
 */

// Header Files
#include <X11/Xlib.h>
#include <GL/freeglut.h>

#pragma comment(lib, "freeglut.lib")

// Global Variable Declarations
Bool bFullscreen = False;

// Entry-point Function
int main(int argc, char *argv[])
{
	// Local Function Declaration
	int initialize(void);
	void resize(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);
	void uninitialize(void);

	// Code
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Mayur Tadekar");

	initialize();

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	return(0);
}

int initialize(void)
{
	// Code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	return(0);
}

void resize(int width, int height)
{
	// Code
	glViewport(0, 0, width, height);
}

void display(void)
{
	// Code
	glClear(GL_COLOR_BUFFER_BIT);


	glBegin(GL_TRIANGLES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glEnd();

	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	// Code
	switch(key)
	{
	case 27:
		glutLeaveMainLoop();
		break;

	case 'f':
	case 'F':
		if(bFullscreen == False)
		{
			glutFullScreen();
			bFullscreen = True;
		}
		else
		{
			glutLeaveFullScreen();
			bFullscreen = False;
		}
		break;

	default:
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	// Code
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		glutLeaveMainLoop();
		break;

	default:
		break;
	}
}

void uninitialize(void)
{
	// Code
}
