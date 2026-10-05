

// HEADERS
#include	<iostream>
#include	<stdio.h>
#include	<stdlib.h>
#include	<memory.h>

#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/XKBlib.h>
#include	<X11/keysym.h>

#include	<GL/gl.h>
#include	<GL/glu.h>
#include	<GL/glx.h>

#include	<SOIL/SOIL.h>

// MACROS
#define		WIN_WIDTH	800
#define		WIN_HEIGHT	600

#define		ERROR_EXIT(error)	{						\
						fprintf(stderr, "ERROR: %s\n", error);	\
						Uninitialize();				\
						exit(1);				\
					}

// GLOBAL VARIABLES
bool		bFullscreen = false;
Display		*gpDisplay = NULL;
XVisualInfo	*gpXVisualInfo = NULL;
Window		gWindow;
Colormap	gColormap;
GLXContext	gGLXContext;

GLuint 	textureMarble = 0;
GLfloat translateCubeX = 0.0f;
GLfloat translateCubeY = 0.0f;
GLfloat translateCubeZ = 0.0f;

GLfloat lightAmbient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 0.0f };

int bLight = 0;

// FUNCTIONS
int main(int argc, char **argv, char **envp) {

	// FUNCTIONS
	void Initialize(void);
	void CreateWindow(void);
	void Uninitialize(void);
	void ToggleFullscreen(void);
	void Resize(int, int);
	void Draw(void);

	// VARIABLES
	int	win_width = 0;
	int 	win_height = 0;
	bool	bDone = false;
	XEvent	event = {0};
	KeySym	keysym;

	// CODE

	CreateWindow();

	Initialize();

	while( bDone == false ) {
		while( XPending(gpDisplay) ) {
			XNextEvent(gpDisplay, &event);

			switch( event.type) {
				case MapNotify:
					break;

				case KeyPress:
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);

					switch(keysym) {
						case XK_Escape:
							bDone = true;
							break;

						case XK_F:
						case XK_f:
							ToggleFullscreen();
							bFullscreen = !bFullscreen;
							break;

						case XK_L:
						case XK_l:
							if( bLight )
							{
								bLight = 0;
								glDisable(GL_LIGHTING);
							}
							else
							{
								bLight = 1;
								glEnable(GL_LIGHTING);
							}
							break;
					}
					break;

				case ButtonPress:
					switch(event.xbutton.button) {
						case 1:
							break;

						case 2:
							break;

						case 3:
							break;

						default:
							break;
					}
					break;

				case ConfigureNotify:
					Resize(	event.xconfigure.width,
						event.xconfigure.height);
					break;

				case Expose:
					break;

				case 33:
					bDone = true;
					break;
			}
		}
		Draw();
	}

	Uninitialize();
	return(0);
}

void CreateWindow(void) {

	// FUNCTIONS
	void Uninitialize(void);

	// VARIABLE
	XSetWindowAttributes	winAttribs;
	int	defaultScreen;
	int	styleMask;
	static	int	frameBufferAttributes[]	= {	GLX_DOUBLEBUFFER,
							GLX_RGBA,
							GLX_RED_SIZE,	8,
							GLX_GREEN_SIZE,	8,
							GLX_BLUE_SIZE,	8,
							GLX_ALPHA_SIZE,	8,
							GLX_DEPTH_SIZE,	24,
							None };

	// CODE

	gpDisplay = XOpenDisplay(NULL);
	if( NULL == gpDisplay)
		ERROR_EXIT("Display cannot be opened\n");

	defaultScreen = XDefaultScreen(gpDisplay);

	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	if( NULL == gpXVisualInfo)
		ERROR_EXIT("Vaild visual cannot be found\n");

	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.background_pixel = BlackPixel( gpDisplay,
						  defaultScreen);

	winAttribs.colormap = XCreateColormap(	gpDisplay,
						RootWindow(gpDisplay, gpXVisualInfo->screen),
						gpXVisualInfo->visual,
						AllocNone);

	winAttribs.event_mask = ExposureMask | StructureNotifyMask | VisibilityChangeMask | PointerMotionMask | ButtonPressMask | KeyPressMask ;

	gColormap = winAttribs.colormap;

	styleMask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

	gWindow = XCreateWindow( gpDisplay,
				 RootWindow( gpDisplay, gpXVisualInfo->screen),
				 0,
				 0,
				 WIN_WIDTH,
				 WIN_HEIGHT,
				 0,
				 gpXVisualInfo->depth,
				 InputOutput,
				 gpXVisualInfo->visual,
				 styleMask,
				 &winAttribs);

	if( 0 == gWindow)
		ERROR_EXIT("Window cannot be created\n");

	XStoreName(gpDisplay, gWindow, "Window");

	Atom windowDeleteManager = XInternAtom(gpDisplay, "WM_DELETE_MANAGER", True);
	XSetWMProtocols(gpDisplay, gWindow, &windowDeleteManager, 1);

	XMapWindow(gpDisplay, gWindow);
}

void Initialize(void) {

	// FUNCTIONS
	void Resize(int, int);
	GLuint LoadBitmapAsTexture(const char *);

	// CODE
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, True);

	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	glShadeModel(GL_SMOOTH);

	glDepthFunc(GL_LEQUAL);

	glEnable(GL_DEPTH_TEST);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearDepth(1.0f);

	glEnable(GL_TEXTURE_2D);

	textureMarble = LoadBitmapAsTexture("./marble.bmp");

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

GLuint	LoadBitmapAsTexture(const char *path) {

	// FUNCTION
	void Uninitialize(void);

	// VARIABLE
	int width = 0;
	int height = 0;
	GLuint	textureId;
	unsigned char *image_data = NULL;

	// CODE
	if( NULL == path )
		ERROR_EXIT("Path invalid");

	image_data = SOIL_load_image(path, &width, &height, NULL, SOIL_LOAD_RGB);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, image_data);

	SOIL_free_image_data(image_data);

	return(textureId);
}


void ToggleFullscreen(void) {

	// VARIABLES
	Atom	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
	Atom	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
	XEvent	event = {0};

	// CODE
	event.type = ClientMessage;
	event.xclient.window = gWindow;
	event.xclient.message_type = wm_state;
	event.xclient.format = 32;
	event.xclient.data.l[0] = bFullscreen ? 0 : 1;
	event.xclient.data.l[1] = fullscreen;

	XSendEvent(	gpDisplay,
			RootWindow(gpDisplay, gpXVisualInfo->screen),
			False,
			StructureNotifyMask,
			&event);
}

void Resize(int width, int height) {

	// CODE
	if( height < 1)
		height = 1;

	glViewport(0, 0, (GLsizei) width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Draw(void)
{
	// Function Declaration
	void drawLitCube();
	void drawFloor(void);

	// Variable Definitions
	static float spin = 0.0f;

	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(	0.0f, 5.0f, 10.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f);

	// Render The Actual Cube
	glPushMatrix();
	{
		glTranslatef(translateCubeX, translateCubeY, translateCubeZ);
		glTranslatef(0.0f, 1.0f, 0.0f);
		glRotatef(spin , 0.0f, 1.0f, 0.0f);
		drawLitCube();
	}
	glPopMatrix();

	// Define you floor as the stencil
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	drawFloor();

	glEnable(GL_DEPTH_TEST);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	// Render The Reflected Cube
	glPushMatrix();
	{
		glScalef(1.0f, -1.0f, 1.0f);
		glTranslatef(translateCubeX, translateCubeY, translateCubeZ);
		glTranslatef(0.0f, 1.0f, 0.0f);
		glRotatef(spin , 0.0f, 1.0f, 0.0f);
		drawLitCube();
	}
	glPopMatrix();

	glDisable(GL_STENCIL_TEST);


	// Render Floor
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1.0f, 1.0f, 1.0f, 0.7f);
	drawFloor();
	glDisable(GL_BLEND);

	spin += 1.0f;

	if( spin > 360.0f)
		spin - 0.0f;

	glXSwapBuffers(gpDisplay, gWindow);

}

void drawLitCube()
{
	// Code
	glEnable(GL_LIGHTING);

	glBegin(GL_QUADS);
	{
		// Front
		glNormal3f(0.0f, 0.0f, 0.3f);
		glVertex3f(-0.3f, 0.3f, 0.3f);
		glVertex3f(-0.3f, -0.3f, 0.3f);
		glVertex3f( 0.3f, -0.3f, 0.3f);
		glVertex3f(0.3f, 0.3f, 0.3f);

		// Right
		glNormal3f(0.3f, 0.0f, 0.0f);
		glVertex3f(0.3f, 0.3f, -0.3f);
		glVertex3f(0.3f, -0.3f, -0.3f);
		glVertex3f(0.3f, -0.3f, 0.3f);
		glVertex3f(0.3f, 0.3f, 0.3f);

		// Back
		glNormal3f(0.0f, 0.0f, -0.3f);
		glVertex3f(-0.3f, 0.3f, -0.3f);
		glVertex3f(-0.3f, -0.3f, -0.3f);
		glVertex3f(0.3f, -0.3f, -0.3f);
		glVertex3f(0.3f, 0.3f, -0.3f);

		// Left
		glNormal3f(-0.3f, 0.0f, 0.0f);
		glVertex3f(-0.3f, 0.3f, -0.3f);
		glVertex3f(-0.3f, -0.3f, -0.3f);
		glVertex3f(-0.3f, -0.3f, 0.3f);
		glVertex3f(-0.3f, 0.3f, 0.3f);

		// Top
		glNormal3f(0.0f, 0.3f, 0.0f);
		glVertex3f(-0.3f, 0.3f, -0.3f);
		glVertex3f(0.3f, 0.3f, -0.3f);
		glVertex3f(0.3f, 0.3f, 0.3f);
		glVertex3f(-0.3f, 0.3f, 0.3f);

		// Bottom
		glNormal3f(0.0f, -0.3f, 0.0f);
		glVertex3f(-0.3f, -0.3f, -0.3f);
		glVertex3f(0.3f, -0.3f, -0.3f);
		glVertex3f(0.3f, -0.3f, 0.3f);
		glVertex3f(-0.3f, -0.3f, 0.3f);
	}
	glEnd();

	glDisable(GL_LIGHTING);
}

void drawFloor(void)
{
	// Code
	glBindTexture(GL_TEXTURE_2D, textureMarble);

	glBegin(GL_QUADS);
	{
		/*	FRONT FACE	*/
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(3.0f, 0.0f, 3.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-3.0f, 0.0f, 3.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-3.0f, 0.0f, -3.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(3.0f, 0.0f, -3.0f);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Uninitialize(void) {

	// CODE
	if( glXGetCurrentContext() == gGLXContext)
		glXMakeCurrent(gpDisplay, 0, 0);

	if( gGLXContext ) {
		glXDestroyContext(gpDisplay, gGLXContext);
	}

	if( textureMarble) {
		glDeleteTextures(1, &textureMarble);
	}

	if( gWindow ) {
 		XDestroyWindow(gpDisplay, gWindow);
	}

	if( gColormap ) {
		XFreeColormap(gpDisplay, gColormap);
	}

	if( gpXVisualInfo ) {
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}

	if( gpDisplay ) {
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}

