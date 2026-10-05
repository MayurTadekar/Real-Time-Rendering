

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

// MACROS
#define		WIN_WIDTH	800
#define		WIN_HEIGHT	600

#define		CHECKERBOARDWIDTH	64
#define		CHECKERBOARDHEIGHT	64

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
GLubyte		checkImage[CHECKERBOARDWIDTH][CHECKERBOARDHEIGHT][4];
GLuint		texImage;

// FUNCTIONS

int main(int argc, char **argv, char **envp) {

	// FUNCTIONS
	void Initialize(void);
	void CreateWindow(void);
	void Uninitialize(void);
	void ToggleFullscreen(void);
	void Draw(void);
	void Resize(int, int);

	// VARIABLES
	XEvent		event = {0};
	KeySym		keysym;
	bool		bDone = false;

	// CODE

	CreateWindow();

	Initialize();

	while( !bDone ) {
		while( XPending(gpDisplay) ) {
			XNextEvent(gpDisplay, &event);

			switch(event.type) {
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
	XSetWindowAttributes		winAttribs;
	int		defaultScreen;
	int		styleMask;
	static int 	frameBufferAttributes[] = {	GLX_DOUBLEBUFFER,
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
		ERROR_EXIT("Dispaly cannot be opened");

	defaultScreen = XDefaultScreen(gpDisplay);

	gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
	if( NULL == gpXVisualInfo)
		ERROR_EXIT("Cannot find vaild Visual");

	winAttribs.border_pixel = 0;
	winAttribs.background_pixmap = 0;
	winAttribs.background_pixel = BlackPixel( gpDisplay,
						  defaultScreen);

	winAttribs.colormap = XCreateColormap(	gpDisplay,
						RootWindow(gpDisplay, gpXVisualInfo->screen),
						gpXVisualInfo->visual,
						AllocNone);

	winAttribs.event_mask = ExposureMask | StructureNotifyMask | VisibilityChangeMask | PointerMotionMask | ButtonPressMask | KeyPressMask;

	gColormap = winAttribs.colormap;

	styleMask = CWBackPixel | CWBorderPixel | CWEventMask | CWColormap;

	gWindow = XCreateWindow( gpDisplay,
				 RootWindow(gpDisplay, gpXVisualInfo->screen),
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
		ERROR_EXIT("Window cannot be created");

	XStoreName(gpDisplay, gWindow, "Checker");

	Atom	windowDeleteManager = XInternAtom( gpDisplay, "WM_DELETE_MANAGER", False);
	XSetWMProtocols(gpDisplay, gWindow, &windowDeleteManager, 1);


	XMapWindow(gpDisplay, gWindow);
}


void Initialize(void) {

	// FUNCTION
	void Resize(int, int);
	void LoadGLTexture(void);

	// CODE
	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, True);

	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	glShadeModel(GL_SMOOTH);

	glDepthFunc(GL_LEQUAL);

	glEnable(GL_DEPTH_TEST);

	glClearDepth(1.0f);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);

	LoadGLTexture();

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

	Resize(WIN_WIDTH, WIN_HEIGHT);
}

void ToggleFullscreen(void) {

	// VARIABLE
	Atom	wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", True);
	Atom	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", True);
	XEvent	event = {0};

	// CODE
	event.type = ClientMessage;
	event.xclient.message_type = wm_state;
	event.xclient.window = gWindow;
	event.xclient.format = 32;
	event.xclient.data.l[0] = bFullscreen ? 0 : 1;
	event.xclient.data.l[1] = fullscreen;


	XSendEvent(	gpDisplay,
			RootWindow(gpDisplay, gpXVisualInfo->screen),
			False,
			StructureNotifyMask,
			&event);
}


void LoadGLTexture(void)
{
	/* FUNCTION SIGNATURE */
	void MakeCheckerBoard(void);

	/* CODE */
	MakeCheckerBoard();
	glGenTextures(1, &texImage);
	glBindTexture(GL_TEXTURE_2D, texImage);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERBOARDWIDTH, CHECKERBOARDHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void MakeCheckerBoard(void)
{
	int c;

	for (int i = 0; i < CHECKERBOARDHEIGHT; ++i)
	{
		/* code */
		for (int j = 0; j < CHECKERBOARDWIDTH; ++j)
		{
			/* code */
			c = ( ( (i & 0x8) == 0 ) ^ ( ( j & 0x8 ) == 0 ) ) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
}


void Resize(int width, int height) {

	// CODE
	if( height < 1)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Draw(void) {

	// CODE

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -5.0f);

	glBindTexture(GL_TEXTURE_2D, texImage);

	glPushMatrix();
	{
		glTranslatef(-1.0f, 0.0f, 0.0f);
		glBegin(GL_QUADS);
		{
			// Left Quad
			glTexCoord2f( 0.0f, 0.0f );
			glVertex3f(-1.0f, -1.0f, 0.0f);

			glTexCoord2f( 1.0f, 0.0f );
			glVertex3f(1.0f, -1.0f, 0.0f);

			glTexCoord2f( 1.0f, 1.0f );
			glVertex3f(1.0f, 1.0f, 0.0f);

			glTexCoord2f( 0.0f, 1.0f );
			glVertex3f(-1.0f, 1.0f, 0.0f);
		}
		glEnd();
	}
	glPopMatrix();


	glPushMatrix();
	{
		glTranslatef(2.0f, 0.0f, 0.0f);
		glScalef(0.9f, 0.9f, 0.9f);
		glRotatef(42.0f, 0.0f, 1.0f, 0.0f);
		glBegin(GL_QUADS);
		{
			// Right Quad
			glTexCoord2f( 0.0f, 0.0f );
			glVertex3f(-1.0f, -1.0f, 0.0f);

			glTexCoord2f( 1.0f, 0.0f );
			glVertex3f(1.0f, -1.0f, 0.0f);

			glTexCoord2f( 1.0f, 1.0f );
			glVertex3f(1.0f, 1.0f, 0.0f);

			glTexCoord2f( 0.0f, 1.0f );
			glVertex3f(-1.0f, 1.0f, 0.0f);
		}
		glEnd();
	}
	glPopMatrix();

	glXSwapBuffers(gpDisplay, gWindow);
}


void Uninitialize(void) {

	// CODE

	if( glXGetCurrentContext() == gGLXContext )
		glXMakeCurrent(gpDisplay, 0, 0);

	if( gGLXContext )
		glXDestroyContext(gpDisplay, gGLXContext);

	if( texImage ) {
		glDeleteTextures(1, &texImage);
	}

	if( gWindow )
		XDestroyWindow(gpDisplay, gWindow);

	if( gColormap )
		XFreeColormap(gpDisplay, gColormap);

	if( gpXVisualInfo ) {
		free(gpXVisualInfo);
		gpXVisualInfo = NULL;
	}

	if( gpDisplay ) {
		XCloseDisplay(gpDisplay);
		gpDisplay = NULL;
	}
}
